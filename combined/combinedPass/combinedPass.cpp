#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DominatorTree.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Timer.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <iomanip>

using namespace llvm;

namespace {
    struct combinedPass : public PassInfoMixin<combinedPass> {
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

        
        int pointerHeuristic(BranchInst* BI) {
            // Ensure the branch is conditional
            if (!BI || !BI->isConditional())
                return 0;

            Value* cond = BI->getCondition();
            
            // Check if the condition is an integer comparison instruction
            if (ICmpInst* icmp = dyn_cast<ICmpInst>(cond)) {
                // Check if operands are pointers
                Value* lhs = icmp->getOperand(0);
                Value* rhs = icmp->getOperand(1);
                if (lhs->getType()->isPointerTy() && rhs->getType()->isPointerTy()) {
                    // Apply heuristic based on comparison type
                    switch (icmp->getPredicate()) {
                        case ICmpInst::ICMP_EQ:
                            // Pointers are not expected to be equal, so return 0
                            return 0;
                        case ICmpInst::ICMP_NE:
                            // Pointers are expected to be not equal, so return 1
                            return 5;  // Encode as '1' indicating more likely
                        default:
                            // For other pointer comparisons, default to a neutral expectation
                            return 0;
                    }
                }
            }
            return 0;
        }
        
        int loopHeuristic(BranchInst* BI, llvm::LoopAnalysis::Result &li) {
            int then = 0;
            int el = 0;
            if (BI->getNumSuccessors() == 2){
                if (li.getLoopFor(BI->getSuccessor(0))){
                    then++;
                }
                if (li.getLoopFor(BI->getSuccessor(1))){
                    el++;
                }
            }
            if (then > el){
                return 4;
            } 
            if (then < el){
                return 0;
            }
            return -1;
        }

        int opcodeHeuristic(BranchInst* BI) {
            if (!BI || !BI->isConditional()) return 0;

            Value* cond = BI->getCondition();

            // Check for integer comparisons
            if (ICmpInst* icmp = dyn_cast<ICmpInst>(cond)) {
                switch (icmp->getPredicate()) {
                    case ICmpInst::ICMP_SLT: // Signed less than
                    case ICmpInst::ICMP_ULT: // Unsigned less than
                        // Ensure that zero comparison is correctly checked regardless of operand order
                        if (ConstantInt* CI = dyn_cast<ConstantInt>(icmp->getOperand(0))) {
                            if (CI->isZero() && icmp->getOperand(1)->getType()->isIntegerTy())
                                return 0;  // Less likely if comparing zero with an integer
                        }
                        if (ConstantInt* CI = dyn_cast<ConstantInt>(icmp->getOperand(1))) {
                            if (CI->isZero())
                                return 0;  // Less likely if comparing an integer with zero
                        }
                        break;
                    default:
                        break;  // No specific likelihood for other comparisons
                }
            }

            // Check for floating point comparisons
            if (FCmpInst* fcmp = dyn_cast<FCmpInst>(cond)) {
                switch (fcmp->getPredicate()) {
                    case FCmpInst::FCMP_OEQ: // Ordered equals
                    case FCmpInst::FCMP_UEQ: // Unordered or equals
                        return 0;  // Floating point equality is less likely
                    default:
                        break;  // No specific likelihood for other comparisons
                }
            }

            return 3;  // Default to more likely for all other conditions
        }

        int guardHeuristic(BranchInst* BI) {
            // Ensure the branch is conditional
            if (!BI || !BI->isConditional())
                return 0;

            Value* cond = BI->getCondition();

            // Check if comparison instruction
            if (ICmpInst* icmp = dyn_cast<ICmpInst>(cond)) {
                Value* lhs = icmp->getOperand(0);
                Value* rhs = icmp->getOperand(1);

                // Detecting if one of the operands is used immediately after the branch
                for (auto user : lhs->users()) {
                    if (Instruction* inst = dyn_cast<Instruction>(user)) {
                        // binaryop and load (dereference) require LHS to be valid
                        if (inst->isBinaryOp() || isa<LoadInst>(inst)) {
                            // Check if the branch likely guards the use of LHS
                            switch (icmp->getPredicate()) {
                                case ICmpInst::ICMP_EQ:
                                    // Expecting the condition to be false to use LHS
                                    return 2;
                                case ICmpInst::ICMP_NE:
                                    // Expecting the condition to be true to use LHS
                                    return 0;
                                default:
                                    return 0;
                            }
                        }
                    }
                }
            }
            return 0;
        }

        int branchDirectionHeuristic(BranchInst* BI) {
            if (!BI || !BI->isConditional())
                return 0;

            // Get metadata
            MDNode* branchMD = BI->getMetadata("BranchDirection");

            if (!branchMD) return 0;

            MDString* directionMD = dyn_cast<MDString>(branchMD->getOperand(0));
            if (!directionMD) return 0;

            StringRef direction = directionMD->getString();

            if (direction.equals("backward")) {
                // likely a loop back edge and should be taken
                return 1;
            } else if (direction.equals("forward")) {
                return 0;
            }
            
            return 0;
        }


        void pathSelection(BasicBlock* BB, std::unordered_set<BasicBlock*>& SuperBlockBB, std::set<BasicBlock*>& finalPath, llvm::PostDominatorTreeAnalysis::Result& PDT, llvm::LoopAnalysis::Result &li){
            if (!BB) return;

            int pathHeuristicCount = 0;
            // finalPath.push_back(BB);
            finalPath.insert(BB);

            for (Instruction& I : *BB) {
                if (auto* BI = dyn_cast<BranchInst>(&I)) {
                    if (BI->isConditional()){
                        BasicBlock *thenBlock = BI->getSuccessor(0);
                        BasicBlock *elseBlock = BI->getSuccessor(1);

                        // check if successors are hazardous
                        // if (hazardBlocks.find(thenBlock) != hazardBlocks.end()) {
                        //     if (hazardBlocks.find(elseBlock) != hazardBlocks.end()) {
                        //         // !thenBlock && !elseBlock
                        //         return;
                        //     } else {
                        //         // !thenBlock, skip heuristic check and go to elseBlock
                        //         if (PDT.dominates(elseBlock, BB)){
                        //             pathSelection(elseBlock, hazardBlocks, finalPath, PDT, li);
                        //         }
                        //     }
                        // } else {
                        //     if (hazardBlocks.find(elseBlock) != hazardBlocks.end()) {
                        //         // !elseBlock, skip heuristic check and go to thenBlock
                        //         if (PDT.dominates(thenBlock, BB)){
                        //             pathSelection(thenBlock, hazardBlocks, finalPath, PDT, li);
                        //         }
                        //     } else {
                        //         // both are safe, go to heuristic checks below
                        //         break;
                        //     }
                        // }

                        // check if successors are hazard-less to be merged
                        if (SuperBlockBB.find(thenBlock) == SuperBlockBB.end()){
                            // thenBlock hazardous
                            if (SuperBlockBB.find(elseBlock) == SuperBlockBB.end()){
                                 // both hazardous
                                 return;
                            } else {
                                pathSelection(elseBlock, SuperBlockBB, finalPath, PDT, li);
                            }
                        } else {
                            if (SuperBlockBB.find(elseBlock) == SuperBlockBB.end()){
                                // boat are safe, check path selection heuristics below
                                break;
                            } else{
                                // elseBlock hazardous
                                pathSelection(thenBlock, SuperBlockBB, finalPath, PDT, li);
                            }
                        }

                        pathHeuristicCount += opcodeHeuristic(BI);
                        pathHeuristicCount += pointerHeuristic(BI);
                        pathHeuristicCount += branchDirectionHeuristic(BI);
                        pathHeuristicCount += guardHeuristic(BI);
                        pathHeuristicCount += loopHeuristic(BI, li);

                        // keeping finalPath just in case
                        if (pathHeuristicCount >= 6){
                            errs() << "choosing likely path to ThenBlock.\n";
                            // finalPath.push_back(thenBlock);
                            finalPath.insert(thenBlock);
                            // recurse to follow path
                            if (PDT.dominates(thenBlock, BB)){
                                pathSelection(thenBlock, SuperBlockBB, finalPath, PDT, li);
                            }
                        } else{
                            errs() << "choosing unlikely path to ElseBlock.\n";
                            // finalPath.push_back(elseBlock);
                            finalPath.insert(elseBlock);
                            // recurse to follow path
                            if (PDT.dominates(elseBlock, BB)){
                                pathSelection(elseBlock, SuperBlockBB, finalPath, PDT, li);
                            }
                        }
                    } else{
                        // unconditional branches, simply add to path
                        BasicBlock* nextBB = BI->getSuccessor(0);
                        errs() << "unconditional branch to next block \n";
                        if (PDT.dominates(nextBB, BB)) {
                            pathSelection(nextBB, SuperBlockBB, finalPath, PDT, li);
                        }
                    }
                }
            }
        }

        void updateBrProb(BasicBlock* BB, std::unordered_set<BasicBlock*>& hazardBlocks){
            if (!BB) return;
            if (hazardBlocks.find(BB) != hazardBlocks.end()) return;

            int pathHeuristicCount = 0;
            // finalPath.push_back(BB);
            finalPath.insert(BB);

            for (Instruction& I : *BB) {
                if (auto* BI = dyn_cast<BranchInst>(&I)) {
                    if (BI->isConditional()){
                        BasicBlock *thenBlock = BI->getSuccessor(0);
                        BasicBlock *elseBlock = BI->getSuccessor(1);

                        pathHeuristicCount += opcodeHeuristic(BI);
                        pathHeuristicCount += pointerHeuristic(BI);
                        pathHeuristicCount += branchDirectionHeuristic(BI);
                        pathHeuristicCount += guardHeuristic(BI);
                        pathHeuristicCount += loopHeuristic(BI, li);

                        // setting new weights based on heuristics
                        llvm::MDBuilder MDB(Builder.getContext());
                        llvm::MDNode* Weights = MDB.createBranchWeights(pathHeuristicCount / 15 * 100, (1 - pathHeuristicCount) / 15 * 100);
                        BI->setMetadata(llvm::LLVMContext::MD_prof, Weights);
                    }
                }
            }

            // recurse on other BBs
            for (BasicBlock* succBB : successors(BB)) {
                updateBrProb(succBB, hazardBlocks);
            }

        }

        PreservedAnalyses run(Function& F, FunctionAnalysisManager& FAM) {
            llvm::Timer timer;
            timer.startTimer();

            llvm::PostDominatorTreeAnalysis::Result& PDT = FAM.getResult<PostDominatorTreeAnalysis>(F);
            llvm::LoopAnalysis::Result &li = FAM.getResult<LoopAnalysis>(F);

            std::unordered_set<BasicBlock*> SuperBlockBB;
            std::unordered_set<BasicBlock*> hazardBlocks;

            BasicBlock* BB = &F.front();
            dfsBasicBlocks(BB, SuperBlockBB, hazardBlocks, PDT);

            // std::vector<BasicBlock*> finalPath;
            std::set<BasicBlock*> finalPath;
            // pathSelection(BB, hazardBlocks, finalPath, PDT, li);
            pathSelection(BB, SuperBlockBB, finalPath, PDT, li);

            // update branch probabilities
            updateBrProb(BB, PDT);

            errs() << "size of hazardBlocks " << hazardBlocks.size() << "\n";
            errs() << "size of finalPath " << finalPath.size() << "\n";
            for (auto block : finalPath) {
                block->printAsOperand(errs(), false);
                errs() << "\n";
            }
            // std::cout << "size of superBlockBB " << SuperBlockBB.size() << std::endl;

            timer.stopTimer();
            llvm::TimeRecord totalTime = timer.getTotalTime();
            
            std::ofstream outputFile;
            outputFile.open("../staticOutput.txt", std::ios::out | std::ios::app);
            
            outputFile << std::fixed << std::setprecision(20);
            outputFile << totalTime.getWallTime() << "\n";
            outputFile.close();

            return PreservedAnalyses::all();
        }
    };
};

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
      LLVM_PLUGIN_API_VERSION, "combinedPass", "v0.1",
      [](PassBuilder& PB) {
        PB.registerPipelineParsingCallback(
          [](StringRef Name, FunctionPassManager& FPM,
          ArrayRef<PassBuilder::PipelineElement>) {
            if (Name == "combined") {
              FPM.addPass(combinedPass());
              return true;
            }
            return false;
          }
        );
      }
    };
}