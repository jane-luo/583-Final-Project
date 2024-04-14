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

#include <unordered_set>
using namespace llvm;

namespace {

struct hazardAvoidancePass : public PassInfoMixin<hazardAvoidancePass> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {

    std::unordered_set<BasicBlock*> SuperBlockBB;

    for (BasicBlock &BB : F) {
        bool goodBB = true;

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
}

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
