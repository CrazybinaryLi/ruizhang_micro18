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
#include "klee/ExprBuilder.h"
#include "klee/util/Assignment.h"
#include "klee/util/ExprPPrinter.h"
#include "klee/util/ExprVisitor.h"
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

#include "expr/Lexer.h"
#include "expr/Parser.h"

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
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Signals.h"
#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/system_error.h"
#endif

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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cxxabi.h>

using namespace llvm;
using namespace klee;
using namespace klee::expr;

  cl::opt<std::string>
    pcFileName("pc-file-name", 
                cl::init("obj_dir/klee-last/test000001.pc"), 
         cl::desc("Path constraint file name"));


void Executor::retrieveConstraints(KInstruction *ki, ref<Expr> value) {
  if (internalStateConstraints.size() == OR1200InternalStates.size() - 1) {
    internalStateConstraints.clear();
    internalStateCount = 0;
  }
  internalStateConstraints.push_back(value);
  return;
}



int Executor::getIndex(std::string s) {
  outs() << "Hello: " << s << "\n";
  if (core == OR1200) {
    std::vector<std::string>::iterator it = std::find(OR1200InternalStates.begin(), 
        OR1200InternalStates.end(), s);
    if (it != OR1200InternalStates.end()) 
      return (it - OR1200InternalStates.begin()); 
  }
  return 0;
}

ref<Expr> Executor::replaceReadExpr(const ref<Expr> &a) {
  // replace the readexpr in a with b
  // copy the new a to c
  ref<Expr> c; 
  std::vector<Expr::CreateArg> args;
//  errs() << "Num: " << a->getNumKids() << "\n";
//  errs() << "a: " << a << "\n";
  for (unsigned i = 0; i < a->getNumKids(); i++) {
//    errs() << a->getKid(i)->getKind() << "\n";
    
    if (a->getKid(i)->getNumKids() < 1) {
      outs() << "Type1: " << a->getKid(i) << "\n";
      args.push_back(a->getKid(i));
      outs() << "End of Type1\n";
    }
    
    else if ((a->getKid(i)->getKind() == Expr::Concat) && 
        (a->getKid(i)->getKid(0)->getKind() == Expr::Read)) {
//      errs() << "Concat: " << a->getKid(i) << "\n";
      const ReadExpr *re = dyn_cast<ReadExpr>(a->getKid(i)->getKid(0));
      int ind = getIndex(re->updates.root->name);
      if (ind == 0) {
        ref<Expr> ce = replaceReadExpr(a->getKid(i));
        // ConcatExpr 
        args.push_back(ce);
      }
      else { 
//        errs() << "Index: " << ind << "\n";
//        errs() << OR1200InternalStates[ind] << "\n";
//        errs() << internalStateConstraints[ind-1] << "\n";
        args.push_back(Expr::CreateArg(internalStateConstraints[ind-1]));
      }
    }
    
    else if (const ReadExpr *re = dyn_cast<ReadExpr>(a->getKid(i))) {
      outs() << "Type2: " << a->getKid(i)  << "\n";
//      errs() << "Hello " << re->index << " " << re->updates.root->name << "\n";
//      outs() << "Type2-2: " << re->index << "\n";
      int ind = getIndex(re->updates.root->name);
      if (ind == 0) {
/*
        const ref<Expr> uindex = replaceReadExpr(re->index);
        const UpdateNode* uhead = re->updates.head;
        if (uhead == NULL) {
          ref<Expr> readexp = ReadExpr::create(re->updates, uindex);
          args.push_back(Expr::CreateArg(a->getKid(i)));
        } else {
          ref<Expr> uvalue = replaceReadExpr(uhead->value);
          UpdateNode* unode(uhead->next, uindex, uvalue);
          UpdateList ulist(re->updates.root, unode);
          uhead = uhead->next;
          while (uhead != NULL) {
            ref<Expr> uvalue = replaceReadExpr(uhead->value);
            const UpdateNode* unext(uhead->next, uindex, uvalue);
            unode->next = unext;
            uhead = uhead->next;
            unode = unode->next;
          }
          ref<Expr> readexp = ReadExpr::create(ulist, uindex);
          args.push_back(readexp);
        }
*/
        args.push_back(a->getKid(i));
      }
      else {
//        outs() << "Index: " << ind << "\n";
//        outs() << OR1200InternalStates[ind] << "\n";
//        outs() << internalStateConstraints[ind-1] << "\n";
        if (re->getWidth() != internalStateConstraints[ind-1]->getWidth()) {
          if (internalStateConstraints[ind-1]->getKind() == Expr::ZExt || 
              internalStateConstraints[ind-1]->getKind() == Expr::SExt) {
            internalStateConstraints[ind-1] = internalStateConstraints[ind-1]->getKid(0);
          }
          else if (ConstantExpr *ce = dyn_cast<ConstantExpr>(
                internalStateConstraints[ind-1])) {
            internalStateConstraints[ind-1] = ce->ZExt(re->getWidth());
          }
        }
//        errs() << "Internal: " << internalStateConstraints[ind-1] << " " << 
//          internalStateConstraints[ind-1]->getKind() << "\n";
        args.push_back(Expr::CreateArg(internalStateConstraints[ind-1]));
      }
    }
    
    else {
      ref<Expr> tmp = a->getKid(i);
      outs() << "Type3: " << tmp << "\n";
      ref<Expr> rep = replaceReadExpr(tmp);
      if (const ExtractExpr *ee = dyn_cast<ExtractExpr>(tmp)) {
        args.push_back(Expr::CreateArg(rep, ee->offset));
      } else {
        args.push_back(Expr::CreateArg(rep));
      }
    }

  }
//  errs() << "numArgs: " << args.size() << "\n";
//  errs() << "Kind: " << a->getKind() << "\n";
  if (a->getKind() == Expr::SExt || 
      a->getKind() == Expr::ZExt || 
      a->getKind() == Expr::Extract) { 
    args.push_back(Expr::CreateArg(a->getWidth()));
  }
  if (a->getKind() == Expr::Constant)
    return a;
  c = Expr::createFromKind(a->getKind(), args);
  return c;
}

bool Executor::pathConstraintSatisfied(ExecutionState &state) {

  // Parse PCFile to Constraints 
  std::string fileName = pcFileName;

  ExprBuilder *Builder = 0;
  Builder = createDefaultExprBuilder();

#if LLVM_VERSION_CODE < LLVM_VERSION(3, 5)
  OwningPtr<MemoryBuffer> MB;
  error_code ec = llvm::MemoryBuffer::getFileOrSTDIN(fileName, MB);
  if (ec) {
    llvm::errs() << "Error: " << ec.message() << "\n";
    return false;
  }
#else 
  auto MBResult = llvm::MemoryBuffer::getFileOrSTDIN(fileName);
  if (!MBResult) {
    llvm::errs() << "Error: " << MBResult.getError().message() << "\n";
    return false;
  }
  std::unique_ptr<MemoryBuffer> &MB = *MBResult;
#endif

  std::vector<Decl*> Decls;

  Parser *P = Parser::Create(fileName, MB.get(), Builder, false);
  P->SetMaxErrors(20);
  while (Decl *D = P->ParseTopLevelDecl()) {
    Decls.push_back(D);
  }

  ConstraintManager toSatCM;
  ref<Expr> q;
  
  for (std::vector<Decl*>::iterator it = Decls.begin(), 
      ie = Decls.end(); it != ie; ++it) {
    Decl *D = *it;
    if (QueryCommand *QC = dyn_cast<QueryCommand>(D)) {
      for (ConstraintManager::const_iterator it = QC->Constraints.begin(); 
          it != QC->Constraints.end(); ++it) {
        ref<Expr> e = *it;
        outs() << "Constraint: " << e << "\n";
        ref<Expr> ne = replaceReadExpr(e);
        outs() << "Constraint(replaced): " << ne << "\n";
        if (ne->getKind() == Expr::Constant) {
          if (!cast<ConstantExpr>(ne)->isTrue()) {
            delete P; 
            delete Builder; 
            return false;
          }
        }
        toSatCM.addConstraint(ne);
      }
      q = QC->Query;
    }
  }
  haltExecution = true;
  // End of Parse PCFileName to Constraints 
  
  // Query Solver 
  bool result;
  bool success = solver->solver->mustBeTrue(Query(toSatCM, q), result);
  // End of Query Solver 

  if (result) {
    haltExecution = true;
  }

  delete P;
  delete Builder;


  return result;
}
