#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <set>

using namespace llvm;

namespace {

struct hazardAvoidancePass : public PassInfoMixin<hazardAvoidancePass> {
  std::set<BasicBlock*> ambiguousStoreBlocks;

  // maybe check if any of a blocks successors have ambiguous stores?
  // then those can be avoided
  void containsAmbiguousStore(Function &F) {
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (std::string(I.getOpcodeName()) == "store") {
          StoreInst* store = dyn_cast<StoreInst>(&I);
          if (!isa<Constant>(store->getPointerOperand())) {
            errs() << "ambiguous store found" << BB << "\n\n";
            errs() << *store << "\n\n";
            ambiguousStoreBlocks.insert(&BB);
            break;
          }
        }
      }
    }
  }

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    std::cout << "Hello function: " << F.getName().str() << std::endl;
    containsAmbiguousStore(F);
    for (auto a : ambiguousStoreBlocks) {
      errs() << a << "\n";
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