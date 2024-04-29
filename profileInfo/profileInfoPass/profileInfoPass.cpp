#include "llvm/IR/Function.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
// #include "llvm/Support/Timer.h"

#include <iostream>
// #include <fstream>
// #include <iomanip>

using namespace llvm;

namespace {

struct profileInfoPass : public PassInfoMixin<profileInfoPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    // llvm::Timer timer;
    // timer.startTimer();

    // auto &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
    // auto &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);

    errs() << "Most frequent basic blocks in function: " << F.getName() << "\n";

    for (BasicBlock &BB : F) {
      uint64_t blockFreq = bfi.getBlockFreq(&BB).getFrequency();
      errs() << "BB Name: " << BB.getName() << ", Frequency: " << blockFreq << "\n";
    }

    // timer.stopTimer();
    // llvm::TimeRecord totalTime = timer.getTotalTime();
    
    // std::ofstream outputFile;
    // outputFile.open("../profileOutput.txt", std::ios::out | std::ios::app);
    
    // outputFile << std::fixed << std::setprecision(20);
    // outputFile << totalTime.getWallTime() << "\n";
    // outputFile.close();

    return PreservedAnalyses::all();
  }
};

} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "profileInfoPass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [&](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "profileInfo") {
            FPM.addPass(profileInfoPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
