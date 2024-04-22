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
        bool postDominates(PostDominatorTree& PDT, BasicBlock* BB1, BasicBlock* BB2) {
            if (PDT.dominates(BB2, BB1)) {
                // errs() << *BB2 << " dominates " << *BB1 << "\n";
                return true;
            }
            return false;
        }

        // Checks whether a BB hass an I/O or function call
        bool containsIOInstOrFunc(BasicBlock& BB) {
            for (Instruction& I : BB) {

                if (auto* Call = dyn_cast<CallInst>(&I)) {
                    return true;
                    //errs() << "This basic block contains an I/O or function call or function return\n";
                }
                else if (auto* Load = dyn_cast<LoadInst>(&I)) {
                    Type* OpType = Load->getPointerOperandType();
                    if (OpType->isPointerTy()) {
                        Value* OpValue = Load->getPointerOperand();
                        if (auto* OpInst = dyn_cast<Instruction>(OpValue)) {
                            if (isa<CallInst>(OpInst)) {
                                return true;
                                //errs() << "This basic block contains an I/O or function return\n";
                            }
                        }

                    }
                }

            }

            return false;
        }

        // Checks whether a BB has an ambiguous store in it
        bool containsAmbiguousStore(BasicBlock& BB) {
            for (Instruction& I : BB) {
                if (auto* SI = dyn_cast<StoreInst>(&I)) {
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
        bool containsIndirectJump(BasicBlock& BB) {
            for (Instruction& I : BB) {
                // Indirect jump in branch
                if (isa<IndirectBrInst>(&I)) {
                    // errs() << "indirect jump in branch: " << I << "\n";
                    return true;
                }
                // Indirect call
                else if (auto* CI = dyn_cast<CallInst>(&I)) {
                    if (CI->isIndirectCall()) {
                        // errs() << "indirect call: " << I << "\n";
                        return true;
                    }
                }
            }
            return false;
        }

        // Checks whether a BB contains synchronization instructions
        bool containsSync(BasicBlock& BB) {
            for (Instruction& I : BB) {
                if (auto* CI = dyn_cast<CallInst>(&I)) {
                    Function* called = CI->getCalledFunction();
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

        void dfsBasicBlocks(BasicBlock* BB, std::unordered_set<BasicBlock*>& SuperBlockBB, 
                std::unordered_set<BasicBlock*>& hazardBlocks, llvm::PostDominatorTreeAnalysis::Result& PDT) {

            SuperBlockBB.insert(BB);
            for (Instruction& I : *BB) {
                if (auto* branch = dyn_cast<BranchInst>(&I)) {
                    for (BasicBlock* Successor : successors(BB)) {
                        BasicBlock& refBB = *Successor;

                        if (containsIOInstOrFunc(refBB) || containsAmbiguousStore(refBB)
                            || containsIndirectJump(refBB) || containsSync(refBB)) {

                            hazardBlocks.insert(Successor);
                        }
                    }
                }
            }

            for (BasicBlock* Successor : successors(BB)) {
                if (!SuperBlockBB.count(Successor)) {
                    if (!hazardBlocks.count(Successor) || postDominates(PDT, BB, Successor)) {
                        dfsBasicBlocks(Successor, SuperBlockBB, hazardBlocks, PDT);
                    }
                }
            }
        }

        PreservedAnalyses run(Function& F, FunctionAnalysisManager& FAM) {
            llvm::PostDominatorTreeAnalysis::Result& PDT = FAM.getResult<PostDominatorTreeAnalysis>(F);

            std::unordered_set<BasicBlock*> SuperBlockBB;
            std::unordered_set<BasicBlock*> hazardBlocks;

            BasicBlock* BB = &F.front();
            dfsBasicBlocks(BB, SuperBlockBB, hazardBlocks, PDT);

            return PreservedAnalyses::all();
        }
    };
};

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
      LLVM_PLUGIN_API_VERSION, "hazardAvoidancePass", "v0.1",
      [](PassBuilder& PB) {
        PB.registerPipelineParsingCallback(
          [](StringRef Name, FunctionPassManager& FPM,
          ArrayRef<PassBuilder::PipelineElement>) {
            if (Name == "hazardAvoidance") {
              FPM.addPass(hazardAvoidancePass());
              return true;
            }
            return false;
          }
        );
      }
    };
}
