#include "llvm/IR/Function.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct ProfInfoPass : public PassInfoMixin<ProfInfoPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    auto &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
    auto &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);

    errs() << "Most frequent basic blocks in function: " << F.getName() << "\n";

    for (BasicBlock &BB : F) {
      uint64_t blockFreq = bfi.getBlockFreq(&BB).getFrequency();
      errs() << "BB Name: " << BB.getName() << ", Frequency: " << blockFreq << "\n";
    }

    return PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "ProfInfoPass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [&](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "profinfo") {
            FPM.addPass(ProfInfoPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
