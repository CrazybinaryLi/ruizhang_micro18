
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"

#include <vector>
#include <string>
#include <queue>
#include <utility>
#include <map>

#include "Executor.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/ExecutionState.h"
#include "VarAnalysis.h"
#include "DependencyGraph.h"

using namespace llvm;
using namespace klee;

bool getBrCondVars(Value* ptr, std::vector<Var> &assertVarSet) {
  std::queue<Value*> ptrQueue;
  ptrQueue.push(ptr);
  Value* p;
  while (!ptrQueue.empty()) {
    p = ptrQueue.front();
    ptrQueue.pop();
    if (isa<GetElementPtrInst>(p)) {
      std::string pStr = valueToStr(p);
      errs() << "Assert Var: " << pStr << "\n";
      assertVarSet.push_back(gepStrToVar(pStr));
      continue;
    }
    if (isa<AllocaInst>(p)) {
      continue;
    }
    if (Instruction *I = dyn_cast<Instruction>(&*p)) {
      for (unsigned int i = 0; i < I->getNumOperands(); i++) {
        Value* opPtr = I->getOperand(i);
//        errs() << "Hello\n";
//        errs() << valueToStr(opPtr) << "\n";
        if (isa<Instruction>(opPtr))
          ptrQueue.push(opPtr);
      }
    }
  }
  return true;
}

bool Executor::getKleeAssertVars() {
  int countSimCycles = 0;
  int rstCycles = 0; // 2 
  int simCycles = 2; // tune this number for pipeline
  for (std::vector<KFunction*>::iterator it = kmodule->functions.begin(), 
      ie = kmodule->functions.end(); it != ie; it ++) {
    KFunction *kf = *it;
    if (kf->function->getName() == "__user_main") {
      for (unsigned i = 0; i < kf->numInstructions; i++) {
        KInstruction *ki = kf->instructions[i];
        if (CallInst* callInst = dyn_cast<CallInst>(&*(ki->inst))) {
          if (callInst->getCalledFunction()->getName() == "_ZN11Vor1200_cpu4evalEv") {
            countSimCycles ++;
            errs() << "countSimCycles: " << "\n";
          }
        }
        if (countSimCycles == 2*(simCycles+rstCycles)) { // pipeline+reset
          if (BranchInst *branchInst = dyn_cast<BranchInst>(&*(ki->inst))) {
            getBrCondVars(branchInst, assertVarSet);
          }
        }  // end of if
      } // end of instruction iterator
    } // end if if
  } // end of module iterator
  return true;
} // end of getKleeAssertVars

bool Executor::coiAnalysis() {
  DGraph dgraph;
  errs() << "Begin building dependency graph\n";
  for (Module::iterator F = kmodule->module->begin(), FE = kmodule->module->end(); 
      F != FE; F++) {
    if (F->getName() == "_ZN11Vor1200_cpu5_evalEP17Vor1200_cpu__Syms") {
      // errs() << F->getName() << "\n";
      for (Function::iterator B = F->begin(), BE = F->end(); B != BE; B++) {
        for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; I++) {
          if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
            outs() << *callInst << "\n";
            std::map<Var, int> fromSet;
            std::map<Var, int> toSet;
            fromSet.clear();
            toSet.clear();
            runVarAnalysis(*kmodule, callInst->getCalledFunction()->getName(), fromSet, toSet, dgraph.dgVarLoc);
            for (std::map<Var, int>::iterator mit = fromSet.begin(); 
                mit != fromSet.end(); mit++) {
              if (mit->second == 1) {
                independentVars[mit->first] = 1;
              }
            }
            for (std::map<Var, unsigned>::iterator mit = dgraph.dgVarLoc.begin();
                mit != dgraph.dgVarLoc.end(); mit++) {
              varLoc[mit->first] = mit->second;
            }
            DGNode node = dgToNode(callInst->getCalledFunction()->getName(), fromSet, toSet);
            // Build dependency graph
            dgraph.dgAddNode(node, independentVars);
          }
        } // end of basicblock iteration
      } // end of function iteration
    } // end of if 
  } // end of mudule iteration
  errs() << "End of building dependency graph\n";
//  dgraph.markInstr(*kmodule, assertVarSet, remainInstrSet, 
//      remainFuncSet, independentVars, independentVarsFromAssert);
  initCarryVars();
  dgraph.markInstr(*kmodule, assertVarSet, remainInstrSet, 
      remainFuncSet, carryVars, independentVarsFromAssert);
  return true;
}

void Executor::getCalledFuncName() {
  for (Module::iterator F = kmodule->module->begin(), FE = kmodule->module->end(); 
      F != FE; F++) {
    if (F->getName() == "_ZN11Vor1200_cpu5_evalEP17Vor1200_cpu__Syms") {
      for (Function::iterator B = F->begin(), BE = F->end(); B != BE; B++) {
        for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; I++) {
          if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {
            funcNameSet[callInst->getCalledFunction()->getName()] = 1;
          }
        }
      }
      break;
    }
  }
  return;
}

bool Executor::isOutOfCoI(CallInst *callInst) {
  Function *fun = callInst->getCalledFunction();
  std::string fName;
  if (fun) 
    fName = fun->getName();
  else
    return false;
  if (remainFuncSet.find(fName) != remainFuncSet.end()) {
    return false;
  }
  if (funcNameSet.find(fName) == funcNameSet.end()) {
    return false;
  }
  return true;
/*  KInstruction *ki = state.pc;
  std::string fName = ki->inst->getParent()->getParent()->getName();
  if (funcNameSet.find(fName) == funcNameSet.end()) { 
    return false;
  }
  if (remainInstrSet.find(ki->info->id) != remainInstrSet.end()) 
    return false;
  if (isa<ReturnInst>(ki->inst)) 
    return false;
  if (isa<AllocaInst>(ki->inst))
    return false;
  return true;*/
}

void Executor::printRemainInstrSet() {
  std::map<unsigned, KInstruction*> idInstrMap;
  for (std::vector<KFunction*>::iterator it = kmodule->functions.begin(); 
      it != kmodule->functions.end(); ++it) {
    KFunction *kf = *it;
    for (unsigned i = 0; i < kf->numInstructions; i++) {
      KInstruction *ki = kf->instructions[i]; 
      idInstrMap[ki->info->id] = ki;
    }
  }
  errs() << "RemainInstrSet size: " << remainInstrSet.size() << "\n";
  outs() << "Print RemainInstrSet...\n";
  for (std::map<unsigned, int>::iterator it = remainInstrSet.begin(); 
      it != remainInstrSet.end(); it++) {
    KInstruction *ki = idInstrMap[it->first];
    outs() << *(ki->inst) << "\n";
    outs() << ki->inst->getParent()->getParent()->getName() << "\n";
  }
}

void Executor::printInstrInFunc(std::string funcName) {
  errs() << "Printing instructions in function: " << funcName << "\n";
  for (Module::iterator F = kmodule->module->begin(), FE = kmodule->module->end(); 
      F != FE; F++) {
    if (F->getName() == funcName) {
      for (Function::iterator B = F->begin(), BE = F->end(); B != BE; B++) {
        for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; I++) {
          outs() << *I << "\n";
	}
      }
    }
  }
  return;
}

void Executor::printInstrCountForCoI() {
  outs() << "Total Func: " << funcNameSet.size() << "\n";
  outs() << "Remain Func: " << remainFuncSet.size() << "\n";
  for (std::map<std::string, int>::iterator rit = remainFuncSet.begin(); 
      rit != remainFuncSet.end(); rit++) {
    errs() << rit->first << "\n";
  }
  int countTotalInstr = 0;
  int countRemainInstr = 0;
  for (std::vector<KFunction*>::iterator it = kmodule->functions.begin(); 
      it != kmodule->functions.end(); it ++) {
    KFunction *kf = *it;
    if (funcNameSet.find(kf->function->getName()) != funcNameSet.end()) {
      countTotalInstr += kf->numInstructions;
    }
    if (remainFuncSet.find(kf->function->getName()) != remainFuncSet.end()) {
      countRemainInstr += kf->numInstructions;
    }
  }
  outs() << "Total Instr: " << countTotalInstr << "\n";
  outs() << "Remain Instr: " << countRemainInstr << "\n";
  return;
}

void Executor::printIndependentVars() {
  int size = 0;
  for (std::map<Var, int>::iterator mit = independentVars.begin(); 
      mit != independentVars.end(); mit++) {
    if (mit->second == 1) {
      outs() << mit->first.className << " " << mit->first.regNo << " " << mit->second << "\n";
      outs() << "Line number: " << varLoc[mit->first] << "\n";
      size ++;
    }
  }
  outs() << "Independent Vars Size: " << size << "\n";
}

void Executor::printIndependentVarsFromAssert() {
  int size = 0;
  for (std::map<Var, int>::iterator mit = independentVarsFromAssert.begin(); 
      mit != independentVarsFromAssert.end(); mit++) {
    if (mit->second == 1) {
      outs() << mit->first.className << " " << mit->first.regNo << "\n";
      outs() << "Line number: " << varLoc[mit->first] << "\n";
      size ++;
    }
  }
  outs() << "Independent Vars from Assert Size: " << size << "\n";
}

void Executor::printAllInstructions() {
  for (Module::iterator F = kmodule->module->begin();
      F != kmodule->module->end(); F++) {
    if (F->getName() == "_ZN11Vor1200_cpu5_evalEP17Vor1200_cpu__Syms") {
      for (Function::iterator B = F->begin(); B != F->end(); B++) {
        for (BasicBlock::iterator I = B->begin(); I != B->end(); I++) {
          outs() << *I << "\n";
        }
      }
    }
  }
  return;
}

void Executor::initCarryVars() {
  Var v;
  v.className = "[32 x i32]";
  v.regNo = " i64 %105";
  carryVars[v] = 1;
  v.regNo = " i64 %82";
  carryVars[v] = 1;
  
  v.className = "[64 x i8]";
  v.regNo = " i64 %377";
  carryVars[v] = 1;
  
  v.className = "class.Vor1200_cpu";
  v.regNo = " i32 10";
  carryVars[v] = 1;
  v.regNo = " i32 11";
  carryVars[v] = 1;
  v.regNo = " i32 19";
  carryVars[v] = 1;
  v.regNo = " i32 20";
  carryVars[v] = 1;
  v.regNo = " i32 21";
  carryVars[v] = 1;
  v.regNo = " i32 22";
  carryVars[v] = 1;
  v.regNo = " i32 24";
  carryVars[v] = 1;
  v.regNo = " i32 29";
  carryVars[v] = 1;
  v.regNo = " i32 3";
  carryVars[v] = 1;
  v.regNo = " i32 30";
  carryVars[v] = 1;
  v.regNo = " i32 31";
  carryVars[v] = 1;
  v.regNo = " i32 32";
  carryVars[v] = 1;
  v.regNo = " i32 39";
  carryVars[v] = 1;
  v.regNo = " i32 43";
  carryVars[v] = 1;
  v.regNo = " i32 44";
  carryVars[v] = 1;
  v.regNo = " i32 45";
  carryVars[v] = 1;
  v.regNo = " i32 49";
  carryVars[v] = 1;
  v.regNo = " i32 50";
  carryVars[v] = 1;
  v.regNo = " i32 57";
  carryVars[v] = 1;
  v.regNo = " i32 58";
  carryVars[v] = 1;
  v.regNo = " i32 59";
  carryVars[v] = 1;
  v.regNo = " i32 66";
  carryVars[v] = 1;
  v.regNo = " i32 67";
  carryVars[v] = 1;
  v.regNo = " i32 68";
  carryVars[v] = 1;
  v.regNo = " i32 69";
  carryVars[v] = 1;
  v.regNo = " i32 70";
  carryVars[v] = 1;
  v.regNo = " i32 71";
  carryVars[v] = 1;
  v.regNo = " i32 72";
  carryVars[v] = 1;
  v.regNo = " i32 8";
  carryVars[v] = 1;
  v.regNo = " i32 9";
  carryVars[v] = 1;

  v.className = "class.Vor1200_cpu_or1200_cpu";
  v.regNo = " i32 114";
  carryVars[v] = 1;
  v.regNo = " i32 102";
  carryVars[v] = 1;
  v.regNo = " i32 101";
  carryVars[v] = 1;
  v.regNo = " i32 122";
  carryVars[v] = 1;
  v.regNo = " i32 116";
  carryVars[v] = 1;
  v.regNo = " i32 117";
  carryVars[v] = 1;
  v.regNo = " i32 150";
  carryVars[v] = 1;
  v.regNo = " i32 124";
  carryVars[v] = 1;
  v.regNo = " i32 108";
  carryVars[v] = 1;
  v.regNo = " i32 109";
  carryVars[v] = 1;

  v.className = "class.Vor1200_cpu_or1200_except";
  v.regNo = " i32 70";
  carryVars[v] = 1;
  v.regNo = " i32 69"; 
  carryVars[v] = 1;
  v.regNo = " i32 53";
  carryVars[v] = 1;
  v.regNo = " i32 72";
  carryVars[v] = 1;
  v.regNo = " i32 73";
  carryVars[v] = 1;
  v.regNo = " i32 68";
  carryVars[v] = 1;
  v.regNo = " i32 35";
  carryVars[v] = 1;
  v.regNo = " i32 78";
  carryVars[v] = 1;
  v.regNo = " i32 77";
  carryVars[v] = 1;

  v.className = "class.Vor1200_cpu_or1200_sprs";
  v.regNo = " i32 55";
  carryVars[v] = 1;

}
