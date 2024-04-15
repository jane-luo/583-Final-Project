#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/MDBuilder.h"
#include <iostream>
#include <unordered_set>
#include <set>

using namespace llvm;

namespace {
  struct hazardAvoidancePass : public PassInfoMixin<hazardAvoidancePass> {
    // std::set<BasicBlock*> ambiguousStoreBlocks;

    // // maybe check if any of a blocks successors have ambiguous stores?
    // // then those can be avoided
    // void containsAmbiguousStore(Function &F) {
    //   for (BasicBlock &BB : F) {
    //     for (Instruction &I : BB) {
    //       if (std::string(I.getOpcodeName()) == "store") {
    //         StoreInst* store = dyn_cast<StoreInst>(&I);
    //         if (!isa<Constant>(store->getPointerOperand())) {
    //           errs() << "ambiguous store found" << BB << "\n\n";
    //           errs() << *store << "\n\n";
    //           ambiguousStoreBlocks.insert(&BB);
    //           break;
    //         }
    //       }
    //     }
    //   }
    // }
    void loopHeuristic(BranchInst* BI, int bitpos, llvm::LoopAnalysis::Result &li, std::vector<int> arr) {
      int i = 0;
      for (BasicBlock* suc : successors(BI)) {
        if (li.getLoopFor(suc)) {
          arr[i] = 1;
        }
        i++;
      }
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      std::unordered_set<BasicBlock*> SuperBlockBB;
      llvm::LoopAnalysis::Result &li = FAM.getResult<LoopAnalysis>(F);
      for (BasicBlock &BB : F) {
        bool goodBB = true;
        for (Instruction &I : BB) {
          const char * name  = I.getOpcodeName();
          std::string n(name);
          if (name == "br" || n == "switch" || n == "indirectbr"){
              BranchInst *BI = dyn_cast<BranchInst>(&I);
              MDNode *Weights = BI->getMetadata(LLVMContext::MD_prof);
              MDBuilder MDB(F.getContext());
              unsigned l = I.getNumSuccessors();
              std::vector<int> loopHeurisitcArray(l, 0);
              std::vector<int> pointerHeurisitcArray(l, 0);
              std::vector<int> opcodeHeurisitcArray(l, 0);
              std::vector<int> guardHeurisitcArray(l, 0);
              std::vector<int> branchHeurisitcArray(l, 0);
              std::vector<std::vector<int>> heurisitcArrays = {branchHeurisitcArray, guardHeurisitcArray, opcodeHeurisitcArray, loopHeurisitcArray, pointerHeurisitcArray};
              BranchInst *branchInst = dyn_cast<BranchInst>(&I);
              loopHeuristic(branchInst, 4, li, loopHeurisitcArray);
              // pointerHeurisitc();
              // opcodeHeurisitc();
              std::vector<int> weights(l, 0);
              for (int i = 0; i < 5; i++) {
                for (int j = 0; j < l; j++){
                  if (heurisitcArrays[i][j] > 0){
                      weights[j] += 1 << i;
                  }
                }
              }
              int chosen = -1;
              int max = -1;
              for (int i = 0; i < l; i++) {
                if (weights[i] > max) {
                  chosen = i;
                  max = weights[i];
                }
              }
              if (chosen != -1){
                Weights = MDB.createBranchWeights(chosen, 10000); //just make it large i guess
                BI->setMetadata(LLVMContext::MD_prof, Weights);
              }

          }

          if (auto *Call = dyn_cast<CallInst>(&I)){
            goodBB = false;
            break;
            //errs() << "This basic block contains an I/O or function call or function return\n";
          }
          else if (auto *Load = dyn_cast<LoadInst>(&I)){
            Type *OpType = Load->getPointerOperandType();
            if (OpType->isPointerTy()){
              Value *OpValue = Load->getPointerOperand();
              if (auto *OpInst = dyn_cast<Instruction>(OpValue)){
                if (isa<CallInst>(OpInst)){
                    goodBB = false;
                    break;
                    //errs() << "This basic block contains an I/O or function return\n";
                }
              }

            }
          }
        }

        if (goodBB){
          SuperBlockBB.insert(&BB);
        }
      }
      return PreservedAnalyses::all();
    }
  };
};

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "hazardAvoidancePass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "hazardAvoidance"){
            FPM.addPass(hazardAvoidancePass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
