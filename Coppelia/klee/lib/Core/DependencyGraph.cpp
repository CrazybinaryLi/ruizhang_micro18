#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Support/CallSite.h"
#include "llvm/ADT/SCCIterator.h"

#include "DependencyGraph.h"
#include "VarAnalysis.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstIterator.h"
#include "klee/Internal/Module/KInstruction.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/ExecutionState.h"



using namespace llvm;
using namespace klee;

DGNode dgToNode(std::string funcName, 
    std::map<Var, int> fromSet, std::map<Var, int> toSet) {
  DGNode node;
  node.funcName = funcName;
  node.fromSet = fromSet;
  node.toSet = toSet;
  return node;
}

DGEdge dgToEdge(DGNode fromNode, DGNode toNode) {
  DGEdge edge;
  edge.fromNode = fromNode;
  edge.toNode = toNode;
  return edge;
}

bool DGraph::dgAddNode(DGNode node, 
    std::map<Var, int> &independentVars) {
  this->dgNodeSet.push_back(node);
  for (std::vector<DGNode>::iterator dit = dgNodeSet.begin(); 
      dit != dgNodeSet.end(); dit++) {
    for (std::map<Var, int>::iterator mit = node.fromSet.begin(); 
        mit != node.fromSet.end(); mit++) {
      if (dit->toSet.find(mit->first) != dit->toSet.end()) {
        if (dit->funcName != node.funcName) {
          if (mit->second == 1)
            independentVars[mit->first] ++;
          DGEdge edge = dgToEdge(*dit, node);
          this->dgEdgeSet[edge] = 1;
        }
      }
    }
    for (std::map<Var, int>::iterator mit = node.toSet.begin();
        mit != node.toSet.end(); mit++) {
      if (dit->fromSet.find(mit->first) != dit->fromSet.end()) {
        if (node.funcName != dit->funcName) {
          if (mit->second == 1)
            independentVars[mit->first] ++;
          DGEdge edge = dgToEdge(node, *dit);
          this->dgEdgeSet[edge] = 1;
        }
      }
    }
  }
  return true;
}

std::string DGraph::shortenFuncName(std::string funcName) {
  std::string shortFuncName;
  std::size_t pos1, pos2;
  if (funcName.find("combo") != std::string::npos) 
    pos1 = funcName.find("combo");
  else if (funcName.find("sequent") != std::string::npos) 
    pos1 = funcName.find("sequent");
  else if (funcName.find("multiclk") != std::string::npos)
    pos1 = funcName.find("multiclk");
  else
    pos1 = 0;
  pos2 = funcName.find("EP17");
  shortFuncName = funcName.substr(pos1, pos2-pos1);
  return shortFuncName;
}

bool DGraph::printNodeSet(std::vector<DGNode> nodeSet) {
  errs() << "Printing: \n";
  for (std::vector<DGNode>::iterator vit = nodeSet.begin(); 
      vit != nodeSet.end(); vit++) {
    errs() << vit->funcName << "\n";
  }
  return true;
}

bool DGraph::printNodeMap(std::map<DGNode, int> nodeMap) {
  errs() << "Printing: \n";
  for (std::map<DGNode, int>::iterator mit = nodeMap.begin();
      mit != nodeMap.end(); mit++) {
    errs() << mit->first.funcName << "\n";
  }
  return true;
}

bool DGraph::printEdgeMap(std::map<DGEdge, int> edgeMap) {
  errs() << "Printing: \n";
  for (std::map<DGEdge, int>::iterator mit = edgeMap.begin(); 
      mit != edgeMap.end(); mit++) {
    outs() << shortenFuncName(mit->first.fromNode.funcName) << " -> " 
      << shortenFuncName(mit->first.toNode.funcName) << "\n";
  }
  return true;
}

bool DGraph::getLeafNodes() {
  for (std::map<DGEdge, int>::iterator mit = this->dgEdgeSet.begin(); 
      mit != this->dgEdgeSet.end(); mit++) {
    std::string toFunc = mit->first.toNode.funcName;
    bool isParent = false;
    bool isLeafNode = false;
    for (std::map<DGEdge, int>::iterator mit2 = this->dgEdgeSet.begin();
        mit2 != this->dgEdgeSet.end(); mit2++) {
      std::string fromFunc = mit2->first.fromNode.funcName;
      if (fromFunc == toFunc) {
        isParent = true;
        break;
      }
    }
    for (std::vector<DGNode>::iterator vit = this->dgLeafNodeSet.begin(); 
        vit != this->dgLeafNodeSet.end(); vit++) {
      if (vit->funcName == toFunc) {
        // already a leafnode, no need to push again
        isLeafNode = true;
        break;
      }
    }
    if (isParent == false && isLeafNode == false) {
      // mit is a new leaf node, add to leafnodeset
      this->dgLeafNodeSet.push_back(mit->first.toNode);
    }
  }
  return true;
}

DGNode DGraph::getNode(std::string funcName) {
  for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin(); 
      vit != this->dgNodeSet.end(); vit ++) {
    if (vit->funcName == funcName)
      return *vit;
  }
  return *this->dgNodeSet.begin();
}

std::map<DGNode, int> DGraph::dgBackwardTracking(DGNode leafnode) {
  std::map<DGNode, int> backTrackingNodes;
  std::queue<DGNode> btQueue;
  btQueue.push(leafnode);
  DGNode currnode;
  std::map<std::string, int> trackedNodes;
  trackedNodes[leafnode.funcName] = 1;
  while (!btQueue.empty()) {
    currnode = btQueue.front();
    btQueue.pop();
    for (std::map<DGEdge, int>::iterator mit = this->dgEdgeSet.begin();
      mit != this->dgEdgeSet.end(); mit++) {
      if (currnode.funcName == mit->first.toNode.funcName) {
        backTrackingNodes[mit->first.fromNode] = 1;
        if (trackedNodes.find(mit->first.fromNode.funcName) == trackedNodes.end()) {
          btQueue.push(mit->first.fromNode);
          trackedNodes[mit->first.fromNode.funcName] = 1;
          errs() << shortenFuncName(mit->first.fromNode.funcName) << 
            " -> " << shortenFuncName(currnode.funcName) << "\n";
        }
      }
    }
  }
  return backTrackingNodes;
}

KFunction* locateFunc(KModule &KM, std::string funcName) {
  for (std::vector<KFunction*>::iterator it = KM.functions.begin(), 
      ie = KM.functions.end(); it != ie; it++) {
    KFunction *kf = *it;
    if (kf->function->getName() == funcName) {
      return kf;
    }
  }
  KFunction *kf = *(KM.functions.begin());
  return kf;
}

std::map<DGNode, int> DGraph::dgBackwardTrackingVars(KModule &KM, 
  DGNode node, Var target, std::map<unsigned, int> &remainInstrSet, 
  std::map<Var, int> &trackedVars, std::map<std::string, int> &remainFuncSet, 
  std::map<Var, int> &depVars) {

  std::map<DGNode, int> backTrackingNodes;
  backTrackingNodes[node] = 1;

  std::queue<std::pair<DGNode, std::map<Var, int> > > btQueue;
  std::pair<DGNode, std::map<Var, int> > currpair;
  std::map<Var, int> dependVarSet;
  std::map<Var, int> ctrlDepVarSet;
  runDepAnalysisInFunc(locateFunc(KM, node.funcName), target, dependVarSet, remainInstrSet, remainFuncSet, dgVarLoc, ctrlDepVarSet);
  currpair = std::make_pair(node, dependVarSet);
  btQueue.push(currpair);

  trackedVars[target] = 1;
  
  while (!btQueue.empty()) {
    currpair = btQueue.front();
    btQueue.pop();
    for (std::map<Var, int>::iterator mit = currpair.second.begin(); 
        mit != currpair.second.end(); mit++) {
      for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin();
          vit != this->dgNodeSet.end(); vit++) {
        // if mit is in vit's toSet, runDepanalysis of mit and vit pair
        // add them to the queue
        if ((vit->toSet.find(mit->first) != vit->toSet.end()) && 
            (trackedVars.find(mit->first) == trackedVars.end())) {
          dependVarSet.clear();
          runDepAnalysisInFunc(locateFunc(KM, vit->funcName), mit->first, dependVarSet, remainInstrSet, remainFuncSet, dgVarLoc, ctrlDepVarSet);
          btQueue.push(make_pair(*vit, dependVarSet));
          backTrackingNodes[*vit] = 1;
          trackedVars[mit->first] = 1;
          for (std::map<Var, int>::iterator mit = dependVarSet.begin(); 
              mit != dependVarSet.end(); mit++) {
            depVars[mit->first] = 1;
          }
        }
      }
    }

  }
  for (std::map<Var, int>::iterator mit = ctrlDepVarSet.begin();
      mit != ctrlDepVarSet.end(); mit++) {
//    errs() << "CTRL: " << mit->first.className << " " << mit->first.regNo << "\n";
    depVars[mit->first] = 1;
  }
  return backTrackingNodes;
}

bool DGraph::markInstr(KModule &KM, std::vector<Var> assertVarSet, 
    std::map<unsigned, int> &remainInstrSet, 
    std::map<std::string, int> &remainFuncSet, 
    std::map<Var, int> &independentVars, 
    std::map<Var, int> &independentVarsFromAssert) {
  std::map<Var, int> trackedVars;
  std::map<Var, int> depVars;
  std::map<DGNode, int> targetNodes;
  for (std::vector<Var>::iterator vit = assertVarSet.begin(); 
      vit != assertVarSet.end(); vit++) {
    locateTargetVar(*vit, targetNodes);
    if (targetNodes.size() == 1) {
      DGNode node = targetNodes.begin()->first;
      if (node.funcName == "Not Found")
        continue;
    }
    for (std::map<DGNode, int>::iterator nit = targetNodes.begin(); 
        nit != targetNodes.end(); nit ++) {
      std::map<DGNode, int> btNodeSet = dgBackwardTrackingVars(KM, nit->first, *vit, remainInstrSet, trackedVars, remainFuncSet, depVars);
    }
    // find vars not the output of other functions
/*    for (std::map<Var, int>::iterator mit = depVars.begin(); 
        mit != depVars.end(); mit++) {
      independentVarsFromAssert[mit->first] = 1;
    }*/
    // find vars that are inside the independentVars
    for (std::map<Var, int>::iterator mit = independentVars.begin(); 
        mit != independentVars.end(); mit++) {
      if (mit->second == 1) {
        if (depVars.find(mit->first) != depVars.end()) {
          independentVarsFromAssert[mit->first] = 1;
        }
      }
    }
    errs() << "depVars size: " << depVars.size() << "\n";

  }
  return true;
}

bool DGraph::dgToDot() {
  std::ofstream dotfile;
  dotfile.open("dg.dot");
  dotfile << "digraph dg{\n";
  // print edge
  for (std::map<DGEdge, int>::iterator mit = this->dgEdgeSet.begin(); 
      mit != this->dgEdgeSet.end(); mit++) {
    std::string fromFunc = shortenFuncName(mit->first.fromNode.funcName);
    std::string toFunc = shortenFuncName(mit->first.toNode.funcName);
    dotfile << fromFunc << " -> " << toFunc << ";\n";
  }
  dotfile << "}\n";
  dotfile.close();
  return true;
}

bool DGraph::dgToDotColorBT(DGNode leafnode) {
  errs() << "**********************************\n";
  errs() << "LeafNode: " << leafnode.funcName << "\n";
  std::ofstream dotfile;
  dotfile.open("dg_color_backtracking.dot");
  dotfile << "digraph dg{\n";
  std::map<DGNode, int> btNodeSet = dgBackwardTracking(leafnode);
  for (std::map<DGNode, int>::iterator mit = btNodeSet.begin();
      mit != btNodeSet.end(); mit ++) {
    std::string fName = shortenFuncName(mit->first.funcName);
    dotfile << fName << " [style=filled,color=\"coral\"]\n";
  }
  for (std::map<DGEdge, int>::iterator mit = this->dgEdgeSet.begin(); 
      mit != this->dgEdgeSet.end(); mit++) {
    std::string fromFunc = shortenFuncName(mit->first.fromNode.funcName);
    std::string toFunc = shortenFuncName(mit->first.toNode.funcName);
    dotfile << fromFunc << " -> " << toFunc << ";\n";
  }
  dotfile << "}\n";
  dotfile.close();
  return true;
}

bool DGraph::printAllToVars() {
  for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin(); 
      vit != this->dgNodeSet.end(); vit ++) {
    for (std::map<Var, int>::iterator mit = vit->toSet.begin(); 
        mit != vit->toSet.end(); mit ++) { 
      mit->first.print();
    }
  }
  return true;
}

void DGraph::locateTargetVar(Var target, std::map<DGNode, int> &targetNodes) {
//  std::size_t pos = target.className.find_last_of("or1200");
//  std::string name = target.className.substr(pos);
  for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin(); 
      vit != this->dgNodeSet.end(); vit ++) {
//    for (std::map<Var, int>::iterator mit = vit->toSet.begin(); 
//        mit != vit->toSet.end(); mit++) {
//      errs() << mit->first.className << "\n";
//      errs() << mit->first.regNo << "\n";
//    }

//    errs() << vit->toSet.size() << "\n";
    if (vit->toSet.find(target) != vit->toSet.end()) {
      errs() << "FuncName: " << vit->funcName << "\n";
      targetNodes[*vit] = 1;
    }
  }
//  errs() << "target: " << target.className << " " << target.regNo << "\n";
  if (targetNodes.size() == 0) {
    errs() << "not found\n";
    DGNode node; 
    node.funcName = "Not Found";
    targetNodes[node] = 1;
  }
  return;
}

bool DGraph::dgToDotColorBTVars(KModule &KM, Var target,
    int &countTotalEdge, int &countColorEdge, 
    int &countTotalNode, int &countColorNode, 
    std::map<unsigned, int> &remainInstrSet) {
  errs() << "***********************************\n";
  target.print();
  std::ofstream dotfile;
  dotfile.open("dg_color_backtracking_var.dot");
  dotfile << "digraph dg{\n";

  std::map<DGNode, int> targetNodes;
  locateTargetVar(target, targetNodes);
  std::map<Var, int> trackedVars;
  std::map<std::string, int> remainFuncSet;
  std::map<Var, int> depVars;
  for (std::map<DGNode, int>::iterator it = targetNodes.begin(); 
      it != targetNodes.end(); it++) {
    std::map<DGNode, int> btNodeSet = dgBackwardTrackingVars(KM, it->first, target, 
      remainInstrSet, trackedVars, remainFuncSet, depVars);
  
//    int countTotalNode = 0;
//    int countColorNode = 0;

    for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin(); 
        vit != this->dgNodeSet.end(); vit++) {
      countTotalNode ++;
    }

    for (std::map<DGNode, int>::iterator mit = btNodeSet.begin(); 
        mit != btNodeSet.end(); mit ++) {
      countColorNode ++;
      std::string fName = shortenFuncName(mit->first.funcName);
      dotfile << fName << " [style=filled,color=\"coral\"]\n";
    }
   
    //printEdgeMap(dgEdgeSet);

//    int countTotalEdge = 0;
//    int countColorEdge = 0;

    for (std::map<DGEdge, int>::iterator mit = this->dgEdgeSet.begin(); 
        mit != this->dgEdgeSet.end(); mit++) {
      countTotalEdge ++;
      std::string fromFunc = shortenFuncName(mit->first.fromNode.funcName);
      std::string toFunc = shortenFuncName(mit->first.toNode.funcName);
      dotfile << fromFunc << " -> " << toFunc;
      if ((btNodeSet.find(mit->first.fromNode) != btNodeSet.end()) && 
          (btNodeSet.find(mit->first.toNode) != btNodeSet.end())) {
        countColorEdge ++;
        dotfile << " [color=\"red\" penwidth=10]";
      }
      dotfile << "\n";
    }
  }
  dotfile << "}\n";
  dotfile.close();
  
//  std::ofstream evalfile;
//  evalfile.open("evaluation.txt");
//  evalfile << "Target: " << target.className << " " << target.regNo << ":\n";
//  evalfile << "Total Edge: " << countTotalEdge << "\n";
//  evalfile << "Color Edge: " << countColorEdge << "\n";
//  evalfile << "Total Node: " << countTotalNode << "\n";
//  evalfile << "Color Node: " << countColorNode << "\n";
//  evalfile.close();
  
  return true;
}


bool DGraph::evalAllToVars(KModule &KM, std::map<unsigned, int> &remainInstrSet) {
  int countColorNode, countColorEdge, countTotalNode, countTotalEdge;
  std::ofstream evalfile;
  evalfile.open("eval.csv");
  evalfile << "Target, TotalEdge, ColorEdge, TotalNode, ColorNode\n";
//  int i = 0;
  for (std::vector<DGNode>::iterator vit = this->dgNodeSet.begin(); 
      vit != this->dgNodeSet.end(); vit ++) {
//    i++;
//    if (i >= 10) {
    for (std::map<Var, int>::iterator mit = vit->toSet.begin(); 
        mit != vit->toSet.end(); mit ++) {
      countTotalEdge = 0;
      countColorEdge = 0;
      countTotalNode = 0;
      countColorNode = 0;
      mit->first.print();
      dgToDotColorBTVars(KM, mit->first, countTotalEdge, countColorEdge, 
          countTotalNode, countColorNode, remainInstrSet);
      evalfile << mit->first.className << " " << mit->first.regNo << ",";
      evalfile << countTotalEdge << ",";
      evalfile << countColorEdge << ",";
      evalfile << countTotalNode << ",";
      evalfile << countColorNode << "\n";
    }
//    }
//    if (i == 20)
//      break;
  }
  evalfile.close();
  return true;
}
