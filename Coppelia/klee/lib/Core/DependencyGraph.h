#ifndef KLEE_DEPENDENCYGRAPH_H
#define KLEE_DEPENDENCYGRAPH_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

#include "VarAnalysis.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/ExecutionState.h"

using namespace llvm;
using namespace klee;

typedef struct funcVarSets {
  std::string funcName;
  std::map<Var, int> fromSet;
  std::map<Var, int> toSet;
} DGNode;

typedef struct edge {
  DGNode fromNode;
  DGNode toNode;
} DGEdge;

static bool operator < (DGEdge e1, DGEdge e2) {
  if (e1.fromNode.funcName < e2.fromNode.funcName)
    return true;
  else if (e1.fromNode.funcName == e2.fromNode.funcName) {
    if (e1.toNode.funcName < e2.toNode.funcName)
      return true;
    else
      return false;
  }
  else
    return false;
}

static bool operator < (DGNode n1, DGNode n2) {
  if (n1.funcName < n2.funcName)
    return true;
  else
    return false;
}

DGNode dgToNode(std::string funcName, std::map<Var, int> fromSet, 
        std::map<Var, int> toSet);

DGEdge dgToEdge(DGNode fromNode, DGNode toNode);

class DGraph {
  public:
    DGraph() {};
    ~DGraph() {};
    std::vector<DGNode> dgNodeSet;
    std::map<DGEdge, int> dgEdgeSet;
    std::vector<DGNode> dgLeafNodeSet;
    std::map<Var, unsigned> dgVarLoc;
    bool getLeafNodes();
    bool dgAddNode(DGNode node, std::map<Var, int> &independentVars);
    bool dgToDot();
    bool printNodeSet(std::vector<DGNode> nodeSet);
    bool printNodeMap(std::map<DGNode, int> nodeMap);
    bool printEdgeMap(std::map<DGEdge, int> edgeMap);
    bool printAllToVars();
    std::map<DGNode, int> dgBackwardTracking(DGNode leafnode);
    std::map<DGNode, int> dgBackwardTrackingVars(KModule &KM, DGNode node, 
        Var target, std::map<unsigned, int> &remainInstrSet, 
        std::map<Var, int> &trackedVars, 
        std::map<std::string, int> &remainFuncSet, 
        std::map<Var, int> &depVars);
    bool dgToDotColorBT(DGNode leafnode);
    bool dgToDotColorBTVars(KModule &KM, Var target, 
        int &countTotalEdge, int &countColorEdge, 
        int &countTotalNode, int &countColorNode, 
        std::map<unsigned, int> &remainInstrSet);
    DGNode getNode(std::string funcName);
    void locateTargetVar(Var target, std::map<DGNode, int> &targetNodes);

    bool evalAllToVars(KModule &KM, std::map<unsigned, int> &remainInstrSet);
    bool markInstr(KModule &KM, std::vector<Var> assertVarSet, 
        std::map<unsigned, int> &remainInstrSet, 
        std::map<std::string, int> &remainFuncSet, 
        std::map<Var, int> &independentVars, 
        std::map<Var, int> &independentVarsFromAssert); 
  private:
    std::string shortenFuncName(std::string funcName);
};

#endif /* KLEE_DEPENDENCYGRAPH_H */
