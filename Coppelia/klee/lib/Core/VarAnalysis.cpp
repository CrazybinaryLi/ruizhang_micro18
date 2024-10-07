#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/User.h"
#include "llvm/DebugInfo.h"

#include <map>
#include <stdio.h>
#include <sstream>
#include <queue>

#include "VarAnalysis.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/ExecutionState.h"

using namespace llvm;
using namespace klee;


bool printVarSet(std::map<Var, int> varset) {
  for (std::map<Var, int>::iterator mit = varset.begin();
      mit != varset.end(); mit++) { 
    errs() << "VarSet: ";
    mit->first.print();
  }
  return true;
}

std::string valueToStr(const Value* value) {
  std::string instStr;
  llvm::raw_string_ostream rso(instStr);
  value->print(rso);
  return instStr;
}

Var gepStrToVar(std::string gepStr) {
  Var gepVar;
  std::size_t pos1, pos2;
  pos1 = gepStr.find("class");
  if (pos1 == std::string::npos)
    pos1 = gepStr.find("[");
  pos2 = gepStr.find("*");
  gepVar.className = gepStr.substr(pos1, pos2-pos1);
  //GetElementPtrInst *gepInst = dyn_cast<GetElementPtrInst>(&*gepValue);
  //gepVar.regNo = valueToStr(gepInst->getOperand(2));
  pos1 = gepStr.find_last_of("i");
  gepVar.regNo = gepStr.substr(pos1-1);
  pos1 = gepVar.regNo.find("!dbg");
  if (pos1 != std::string::npos)
    gepVar.regNo = gepVar.regNo.substr(0, pos1-2);
  return gepVar;
}


bool addtoFromToSet(Var fromtoVar, std::map<Var, int> &fromSet, 
    std::map<Var, int> &toSet, int fromto) {
  // fromto = 1, fromtoVar is fromVar
  // fromto = 2, fromtoVar is toVar
  if (fromto == 1) {
    // When we count the independent vars, if the Var is already in the toSet
    // we don't need to add it to the fromSet and we need to delete it in the toSet
    if (toSet.find(fromtoVar) != toSet.end()) {
      toSet[fromtoVar] = 0;
      fromSet[fromtoVar] = 2;
    } else {
      fromSet[fromtoVar] = 1;
    }
  }
  else if (fromto == 2) {
    toSet[fromtoVar] = 1;
  }
  return true;
}


bool getFromToVars(std::string calleeName, Value* fromtoPtr, int fromto, 
    std::map<Var, int> &fromSet, std::map<Var, int> &toSet, 
    std::map<Var, unsigned> &varLoc) {
  std::queue<Value*> ptrQueue;
  ptrQueue.push(fromtoPtr);
  Value* ptr;
  while (!ptrQueue.empty()) {
    ptr = ptrQueue.front();
    ptrQueue.pop();
    if (isa<GetElementPtrInst>(ptr)) {
      std::string ptrStr = valueToStr(ptr);
      addtoFromToSet(gepStrToVar(ptrStr), fromSet, toSet, fromto);
      // debug info
      Instruction *I = dyn_cast<Instruction>(ptr);
      varLoc[gepStrToVar(ptrStr)] = I->getDebugLoc().getLine();
      // end of debug info
      continue;
    }
    if (isa<AllocaInst>(ptr)) {
      continue;
    }
    if (Instruction *I = dyn_cast<Instruction>(&*ptr)) {
      for (unsigned int i = 0; i < I->getNumOperands(); i++) {
        Value* opPtr = I->getOperand(i);
        if (isa<Instruction>(opPtr))
          ptrQueue.push(opPtr);
      }
    }
  }
  return true;
}


bool runVarAnalysis(KModule &KM, std::string calleeName, 
    std::map<Var, int> &fromSet, std::map<Var, int> &toSet, 
    std::map<Var, unsigned> &varLoc) {
  for (std::vector<KFunction*>::iterator it = KM.functions.begin(), 
      ie = KM.functions.end(); it != ie; it++) {
    KFunction *kf = *it;
    if (kf->function->getName() == calleeName) {

      for (unsigned i = 0; i < kf->numInstructions; i++) {
        KInstruction *ki = kf->instructions[i];
        if (StoreInst *storeInst = dyn_cast<StoreInst>(&*(ki->inst))) {
          if (!isa<AllocaInst>(storeInst->getOperand(0)) && 
              !isa<AllocaInst>(storeInst->getOperand(1))) {
            getFromToVars(calleeName, storeInst->getOperand(0), 1, fromSet, toSet, varLoc);
            getFromToVars(calleeName, storeInst->getOperand(1), 2, fromSet, toSet, varLoc);
          }
        }
//        else if (BranchInst *branchInst = dyn_cast<BranchInst>(&*(ki->inst))) {
//          getFromToVars(calleeName, branchInst, 1, fromSet, toSet);
//        }
      }

      break;
    }
  }
//  errs() << calleeName << "\n";
//  errs() << "End fromSet: \n";
//  printVarSet(fromSet);
//  errs() << "End toSet: \n";
//  printVarSet(toSet);
  return true;
}



bool getDataDependVars(Value* toPtr, std::map<Var, int> &depVarSet, 
    std::map<Var, unsigned> &varLoc) {
  std::queue<Value*> ptrQueue;
  ptrQueue.push(toPtr);
  Value* ptr;
  while (!ptrQueue.empty()) {
    ptr = ptrQueue.front();
    ptrQueue.pop();
    if (isa<GetElementPtrInst>(ptr)) {
      std::string ptrStr = valueToStr(ptr);
      Var gepvar = gepStrToVar(ptrStr);
      depVarSet[gepvar] = 1;
      // debug info
      Instruction *I = dyn_cast<Instruction>(ptr);
      varLoc[gepvar] = I->getDebugLoc().getLine();
      // end of debug info
      continue;
    }
    if (isa<AllocaInst>(ptr)) {
      continue;
    }
    if (Instruction *I = dyn_cast<Instruction>(&*ptr)) {
      for (unsigned int i = 0; i < I->getNumOperands(); i++) {
        Value* opPtr = I->getOperand(i);
        if (isa<Instruction>(opPtr)) {
          ptrQueue.push(opPtr);
        }
      }
    }
  }
  return true;
}

bool isVarsInLabel(std::string lbStr, std::map<Var, int> depVarSet) {
  std::size_t pos1, pos2;
  std::string gepStr;
  while (lbStr.find("getelementptr") != std::string::npos) { 
    pos1 = lbStr.find("getelementptr");
    lbStr = lbStr.substr(pos1);
    pos2 = lbStr.find("\n");
    if (pos2 != std::string::npos) {
      gepStr = lbStr.substr(0, pos2);
    }
    else 
      return false;
    lbStr = lbStr.substr(pos2);
    // convert gepStr to var
    Var gepVar = gepStrToVar(gepStr);
    // gepVar.print();
    // check if it's in depVarSet
    if (depVarSet.find(gepVar) != depVarSet.end())
      return true;
  }
  return false;
}

bool getCtrlDependVars(KFunction *KF, std::map<Var, int> &depVarSet, 
    std::map<Var, unsigned> &varLoc) { 
  for (unsigned i = 0; i < KF->numInstructions; i++) {
    KInstruction *ki = KF->instructions[i];
    if (BranchInst *bInst = dyn_cast<BranchInst>(&*(ki->inst))) {
      for (unsigned int i = 0; i < ki->inst->getNumOperands(); i++) {
        std::string opStr = valueToStr(bInst->getOperand(i));
        if (opStr.find("label") != std::string::npos) {
          if (isVarsInLabel(opStr, depVarSet)) {
            // back track branch inst
            // add to depVarSet
            if (isa<Instruction>(bInst->getOperand(0))) {
              getDataDependVars(bInst->getOperand(0), depVarSet, varLoc);
            }
          }
        }
      }
    } // end of if

  }
  return true;
}

bool addtoRemainInstrSet(Value* v, std::map<unsigned, int> &remainInstrSet, 
    KFunction *KF) {
  Instruction *instr = dyn_cast<Instruction>(&*v);
  unsigned id;
  if (KF->instrIdMap.find(instr) != KF->instrIdMap.end())
    id = KF->instrIdMap[instr];
  else
    return false;
  KInstruction *ki = KF->instructions[id];
  remainInstrSet[ki->info->id] = 1;
//  errs() << ki->info->id << " " << *(ki->inst) << "\n";
  return true;
}

bool isinRemainInstrSet(Value* v, std::map<unsigned, int> &remainInstrSet, 
    KFunction *KF) {
  Instruction *instr = dyn_cast<Instruction>(&*v);
  unsigned id;
  if (KF->instrIdMap.find(instr) != KF->instrIdMap.end()) 
    id = KF->instrIdMap[instr];
  else
    return false;
  if (remainInstrSet.find(id) != remainInstrSet.end())
    return true;
  return false;
}

void addtoRemainFuncSet(Value* v, std::map<std::string, int> &remainFuncSet, 
    KFunction *KF) {
  remainFuncSet[KF->function->getName()] = 1;
  return;
}

bool getDataDependVarsInstr(Value* toPtr, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, KFunction *KF, 
    int operandNum, std::map<std::string, int> &remainFuncSet, 
    std::map<Var, unsigned> &varLoc) {
//  std::map<std::string, StoreInst*> storeInstMap;
//  for (unsigned i = 0; i < KF->numInstructions; i++) {
//    KInstruction *ki = KF->instructions[i]; 
//    if (StoreInst *sInst = dyn_cast<StoreInst>(&*(ki->inst))) {
//      storeInstMap[valueToStr(sInst->getOperand(0))] = sInst;
//      storeInstMap[valueToStr(sInst->getOperand(1))] = sInst;
//    }
//  }
  std::queue<Value*> ptrQueue;
  ptrQueue.push(toPtr);
  Value* ptr;
  unsigned gepFlag = 0;
  while (!ptrQueue.empty()) {
    ptr = ptrQueue.front();
    ptrQueue.pop();
    if (isinRemainInstrSet(ptr, remainInstrSet, KF))
      continue;
    addtoRemainInstrSet(ptr, remainInstrSet, KF);
    addtoRemainFuncSet(ptr, remainFuncSet, KF);
    if (isa<GetElementPtrInst>(ptr)) {
      gepFlag = 1 - gepFlag;
      std::string ptrStr = valueToStr(ptr);
      Var gepvar = gepStrToVar(ptrStr);
      // Debug info
      Instruction *I = dyn_cast<Instruction>(ptr);
      varLoc[gepvar] = I->getDebugLoc().getLine();
      // End of debug info
      //if (gepFlag == 1)
      if ((gepvar.className.find("Syms") == std::string::npos) && 
          (operandNum == 0))
        depVarSet[gepvar] = 1;
    }
    if (Instruction *I = dyn_cast<Instruction>(&*ptr)) {
      for (unsigned int i = 0; i < I->getNumOperands(); i++) {
        Value* opPtr = I->getOperand(i); 
        if (isa<Instruction>(opPtr)) {
/*          if (isinRemainInstrSet(opPtr, remainInstrSet, KF)) 
            continue;
          std::string opStr = valueToStr(opPtr);
          // check if it is an operand in a STORE instruction
          if (storeInstMap.find(opStr) != storeInstMap.end()) {
            StoreInst *sInst = storeInstMap[opStr];
            if (!isinRemainInstrSet(sInst, remainInstrSet, KF)) {
              addtoRemainInstrSet(sInst, remainInstrSet, KF);
              ptrQueue.push(sInst->getOperand(0));
              ptrQueue.push(sInst->getOperand(1));
            }
          }
          else {
*/
            ptrQueue.push(opPtr);
//          }
        }
      
      }
    }
  }
  return true;
}


bool getCtrlDependVarsInstr(KFunction *KF, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, std::map<std::string, int> &remainFuncSet, 
    std::map<Var, int> &ctrlDepVarSet) {
  std::map<Var, unsigned> fakeVarLoc;
  for (unsigned i = 0; i < KF->numInstructions; i++) {
    KInstruction *ki = KF->instructions[i];
    if (BranchInst *bInst = dyn_cast<BranchInst>(&*(ki->inst))) {
//      if (bInst->isUnconditional()) { 
//        outs() << "Unconditional: " << *bInst << "\n";
//        outs() << "Successor: " << *(bInst->getSuccessor(0)) << "\n";
//        outs() << "Parent: " << *(bInst->getParent()) << "\n";
//      } else {
//        outs() << "Conditional: " << *bInst << "\n";
//        outs() << "Condition: " << *(bInst->getCondition()) << "\n";
//	outs() << "Successor: " << *(bInst->getSuccessor(0)) << "\n";
//	outs() << "Parent: " << *(bInst->getParent()) << "\n";
//      }
      if (!bInst->isUnconditional()) {
        getDataDependVars(bInst->getCondition(), ctrlDepVarSet, fakeVarLoc);
      }
      if (isinRemainInstrSet(bInst, remainInstrSet, KF))
          continue;
      for (unsigned int i = 0; i < ki->inst->getNumOperands(); i++) {
        std::string opStr = valueToStr(bInst->getOperand(i));
        if (opStr.find("label") != std::string::npos) {
          if (isVarsInLabel(opStr, depVarSet)) {
            addtoRemainInstrSet(bInst, remainInstrSet, KF);
            addtoRemainFuncSet(bInst, remainFuncSet, KF);
          }
        }
        else {
          getDataDependVarsInstr(bInst->getOperand(i), depVarSet, remainInstrSet, KF, 0, remainFuncSet, fakeVarLoc);
        }
      }
    }
  }
  return true;
}

bool runDepAnalysisInFunc(KFunction *KF, Var target, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, std::map<std::string, int> &remainFuncSet, 
    std::map<Var, unsigned> &varLoc, std::map<Var, int> &ctrlDepVarSet) {
  errs() << "Running dep analysis in func: " << KF->function->getName() << "\n";
  
  for (unsigned i = 0; i < KF->numInstructions; i++) {
    KInstruction *ki = KF->instructions[i];
    if (StoreInst *sInst = dyn_cast<StoreInst>(&*(ki->inst))) {
      // errs() << i << " " << KF->instrIdMap[ki->inst] << "\n";
      if (isa<GetElementPtrInst>(sInst->getOperand(1))) {
        std:: string toStr = valueToStr(sInst->getOperand(1));
        // errs() << toStr << "\n";
        Var toVar = gepStrToVar(toStr);
        // Debug info
        Instruction *I = dyn_cast<Instruction>(sInst->getOperand(1));
        varLoc[toVar] = I->getDebugLoc().getLine();
        // End of debug info
        if ((toVar.className == target.className) && 
            (toVar.regNo == target.regNo) && 
            (!isa<AllocaInst>(sInst->getOperand(0))) && 
            (!isa<AllocaInst>(sInst->getOperand(1)))) {
            addtoRemainInstrSet(ki->inst, remainInstrSet, KF);
            getDataDependVarsInstr(sInst->getOperand(0), depVarSet, remainInstrSet, KF, 0, remainFuncSet, varLoc);
//            getDataDependVarsInstr(sInst->getOperand(1), depVarSet, remainInstrSet, KF, 1);
        } // end of if
      } // end of if isa<gep>
    } // end of if storeInst
  } // end of for

  getCtrlDependVarsInstr(KF, depVarSet, remainInstrSet, remainFuncSet, ctrlDepVarSet);
  //printVarSet(depVarSet);
  return true;
}

