#include "Executor.h"
#include "Context.h"
#include "CoreStats.h"
#include "ExternalDispatcher.h"
#include "ImpliedValue.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PTree.h"
#include "Searcher.h"
#include "SeedInfo.h"
#include "SpecialFunctionHandler.h"
#include "StatsTracker.h"
#include "TimingSolver.h"
#include "UserSearcher.h"
#include "ExecutorTimerInfo.h"
#include "VarAnalysis.h"
#include "DependencyGraph.h"

#include "klee/ExecutionState.h"
#include "klee/Expr.h"
#include "klee/Interpreter.h"
#include "klee/TimerStatIncrementer.h"
#include "klee/CommandLine.h"
#include "klee/Common.h"
#include "klee/util/Assignment.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/util/ExprSMTLIBPrinter.h"
#include "klee/util/ExprUtil.h"
#include "klee/util/GetElementPtrTypeIterator.h"
#include "klee/Config/Version.h"
#include "klee/Internal/ADT/KTest.h"
#include "klee/Internal/ADT/RNG.h"
#include "klee/Internal/Module/Cell.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/Internal/Support/ErrorHandling.h"
#include "klee/Internal/Support/FloatEvaluation.h"
#include "klee/Internal/System/Time.h"
#include "klee/Internal/System/MemoryUsage.h"
#include "klee/SolverStats.h"

#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 3)
#include "llvm/IR/Function.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/TypeBuilder.h"
#else
#include "llvm/Attributes.h"
#include "llvm/BasicBlock.h"
#include "llvm/Constants.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#if LLVM_VERSION_CODE <= LLVM_VERSION(3, 1)
#include "llvm/Target/TargetData.h"
#else
#include "llvm/DataLayout.h"
#include "llvm/TypeBuilder.h"
#endif
#endif
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/raw_ostream.h"

#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
#include "llvm/Support/CallSite.h"
#else
#include "llvm/IR/CallSite.h"
#endif

#ifdef HAVE_ZLIB_H
#include "klee/Internal/Support/CompressionStream.h"
#endif

#include <cassert>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <sys/mman.h>

#include <errno.h>
#include <cxxabi.h>

using namespace llvm;
using namespace klee;

snapshot Executor::createSnapshot(ExecutionState &state) {
  snapshot sn;
  sn.ki = state.pc;
  sn.inst_id = sn.ki->info->id;
  sn.stack = state.stack;
  for (ConstraintManager::const_iterator it = state.constraints.begin(); 
      it != state.constraints.end(); ++it) {
    sn.constraints.addConstraint(*it);
  }
  for (MemoryMap::iterator mit = state.addressSpace.objects.begin(); 
      mit != state.addressSpace.objects.end(); ++mit) {
    const MemoryObject * mo = mit->first;
    ObjectState *os = mit->second;
    ref<Expr> stateExpr = os->read(0, os->size * 8);
    sn.memObjs[mo->allocSite] = stateExpr;
  }
  return sn;
}

void Executor::enablePrune() {
  pruneBlacklist["memset"] = 1;
  pruneBlacklist["memcpy"] = 1;
  return;
}

void Executor::printSnapshot(snapshot &sn) {
  errs() << "Instruction id: " << sn.ki->info->id << "\n";
  errs() << "Stack size: " << sn.stack.size() << "\n";
  for (std::vector<StackFrame>::iterator it = sn.stack.begin(); 
      it != sn.stack.end(); ++it) {
    errs() << "StackFrame: " << it->kf->function->getName() << "\n";
  }
  for (ConstraintManager::const_iterator it = sn.constraints.begin(); 
      it != sn.constraints.end(); ++it) {
    errs() << "Constraints: " << *it << "\n";
  }
  
  return;
}

bool Executor::isDuplicate(snapshot sn) {
  if (snapshotHistory.size() == 0)
    return false;
  std::string funcName = sn.ki->inst->getParent()->getParent()->getName();
  if (pruneBlacklist.find(funcName) != pruneBlacklist.end()) 
    return false;
  for (std::vector<snapshot>::iterator sit = snapshotHistory.begin(); 
      sit != snapshotHistory.end(); ++sit) {
    if (sit->inst_id != sn.inst_id) 
      continue;
    if (sit->stack.size() != sn.stack.size()) 
      continue;
    if (sit->constraints.size() != sn.constraints.size()) 
      continue;

    std::vector<StackFrame>::iterator it1 = sn.stack.begin(); 
    std::vector<StackFrame>::iterator it2 = sit->stack.begin();
    bool flag = true;
    for (; it1 != sn.stack.end(), it2 != sit->stack.end(); 
        ++it1, ++it2) {
      if (it1->kf->function->getName() != it2->kf->function->getName()) {
        flag = false;
        break;
      }
    }
    if (flag == false) 
      continue;

    if (sit->constraints == sn.constraints) {
    }
    else {
      continue;
    }

    for (std::map<const llvm::Value*, ref<Expr> >::iterator it = sn.memObjs.begin(); 
        it != sn.memObjs.end(); it++) {
      if (sit->memObjs.find(it->first) == sit->memObjs.end()) {
        flag = false;
        break;
      }
      else {
        if (sit->memObjs[it->first] != it->second) {
          flag = false;
          break;
        }
      }
    }
    if (flag == false)
      continue;

    /*** print debug info ***/
    errs() << "State 1...\n";
    printSnapshot(*sit);
    errs() << "State 2...\n";
    printSnapshot(sn);
    /*** end of debug info ***/
    return true;
  }
  return false;
}

void Executor::addtoSnapshotHistory(snapshot sn) {
  snapshotHistory.push_back(sn);
  return;
}

bool Executor::atBBLPoint(KInstruction *ki) {
  llvm::Instruction *instr = ki->inst->getParent()->getFirstNonPHI();
  if (instr == ki->inst) {
    return true;
  }
  return false;
}
