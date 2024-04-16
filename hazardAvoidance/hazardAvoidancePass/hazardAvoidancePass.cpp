#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <unordered_set>

using namespace llvm;

/*
TO DO:
- figure out how to combine heuristics fluidly
- merging BBs together into superblocks by using the heuristics
  - we check for a conditional branch and then run a heuristic function on the target blocks to detect hazard
- also need to account for loops in function
*/

namespace {
  struct hazardAvoidancePass : public PassInfoMixin<hazardAvoidancePass> {
    // Check if BB2 post dominates BB1
    bool postDominates(PostDominatorTree &PDT, BasicBlock *BB1, BasicBlock *BB2) {
      if (PDT.dominates(BB2, BB1)) {
        errs() << *BB2 << " dominates " << *BB1 << "\n";
        return true;
      }
      return false;
    }

    // Checks whether a BB has an ambiguous store in it
    bool containsAmbiguousStore(BasicBlock &BB) {
      for (Instruction &I : BB) {
        if (auto *SI = dyn_cast<StoreInst>(&I)) {
          // If pointer operand doesn't have a constant value, it's ambiguous
          if (!isa<Constant>(SI->getPointerOperand())) {
            // errs() << "ambiguous store found: " << I << "\n";
            return true;
          }
        }
      }
      return false;
    }

    // Checks whether a BB has an instruction with an indirect jump
    bool containsIndirectJump(BasicBlock &BB) {
      for (Instruction &I : BB) {
        // Indirect jump in branch
        if (isa<IndirectBrInst>(&I)) {
          // errs() << "indirect jump in branch: " << I << "\n";
          return true;
        }
        // Indirect call
        else if (auto *CI = dyn_cast<CallInst>(&I)) {
          if (CI->isIndirectCall()) {
            // errs() << "indirect call: " << I << "\n";
            return true;
          }
        }
      }
      return false;
    }

    // Checks whether a BB contains synchronization instructions
    bool containsSync(BasicBlock &BB) {
      for (Instruction &I : BB) {
        if (auto *CI = dyn_cast<CallInst>(&I)) {
          Function *called = CI->getCalledFunction();
          if (called != nullptr) {
            std::string functionName = std::string(called->getName());
            if (functionName.find("pthread") != std::string::npos || 
                functionName.find("mutex") != std::string::npos ||
                functionName.find("lock") != std::string::npos) {
              // errs() << "contains synchronization instruction\n"; 
              return true;
            }
          }
        }
        // Atomic orderings/memory stuff are no good
        else if (isa<AtomicCmpXchgInst>(&I) || isa<AtomicRMWInst>(&I) || isa<FenceInst>(&I)) {
          // errs() << "contains atomic\n";
          return true;
        }
      }
      return false;
    }

    // jane's scuffed setup function for testing and combining passes
    /*
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      llvm::PostDominatorTreeAnalysis::Result &PDT = FAM.getResult<PostDominatorTreeAnalysis>(F);

      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (auto *branch = dyn_cast<BranchInst>(&I)) {
            if (branch->isConditional()) {
              auto *trueBlock = dyn_cast<BasicBlock>(branch->getOperand(2));
              auto *falseBlock = dyn_cast<BasicBlock>(branch->getOperand(1));

              // needs some logic handling here
              // (if only one function returns false, then pick that one?)
              // (also need to check for post dominator on the block that returns false)

              // containsAmbiguousStore(*trueBlock);
              // containsIndirectJump(*trueBlock);
              // containsSync(*trueBlock);
            }
          }
        }
        // testing whether post dominator function works
        // for (BasicBlock* suc : successors(&BB)) {
        //   postDominates(PDT, &BB, suc);
        // }
      }
      return PreservedAnalyses::all();
    }
    */

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      std::unordered_set<BasicBlock*> SuperBlockBB;

      for (BasicBlock &BB : F) {
        bool goodBB = true;
        containsIndirectJump(BB);

        for (Instruction &I : BB) {

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
