#ifndef KLEE_VARANALYSIS_H
#define KLEE_VARANALYSIS_H

#include "llvm/IR/Module.h"
#include <stdio.h>
#include <string>
#include <queue>

#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/ExecutionState.h"

using namespace llvm;
using namespace klee;

typedef struct gepVariable {
  std::string className;
  std::string regNo;
  void print() const {
    errs() << "Var: \"" << className << "\" \"" << regNo << "\"\n";
  }
} Var;

static bool operator < (Var var1, Var var2) {
  if (var1.className < var2.className) 
    return true;
  else if (var1.className == var2.className) {
    if (var1.regNo < var2.regNo) 
      return true;
    else
      return false;
  }
  else
    return false;
}

bool printVarSet(std::map<Var, int> varset);

bool runVarAnalysis(KModule &KM, std::string calleeName, 
    std::map<Var, int> &fromSet, std::map<Var, int> &toSet, 
    std::map<Var, unsigned> &varLoc);

bool getDataDependVars(Value* toPtr, std::map<Var, int> &depVarSet, 
    std::map<Var, unsigned> &varLoc);
bool getCtrlDependVars(KFunction *KF, std::map<Var, int> &depVarSet, 
    std::map<Var, unsigned> &varLoc);
bool getDataDependVarsInstr(Value* toPtr, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, KFunction *KF, 
    std::map<std::string, int> &remainFuncSet, std::map<Var, unsigned> &varLoc);
bool getCtrlDependVarsInstr(KFunction*KF, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, std::map<std::string, int> &remainFuncSet, 
    std::map<Var, int> &ctrlDepVarSet);
bool runDepAnalysisInFunc(KFunction *KF, Var target, std::map<Var, int> &depVarSet, 
    std::map<unsigned, int> &remainInstrSet, std::map<std::string, int> &remainFuncSet, 
    std::map<Var, unsigned> &varLoc, std::map<Var, int> &ctrlDepVarSet);

std::string valueToStr(const Value* value);
Var gepStrToVar(std::string gepStr);

#endif /* KLEE_VARANALYSIS_H */
