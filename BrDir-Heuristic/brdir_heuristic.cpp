#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

int branchDirectionHeuristic(BranchInst* BI, unsigned int bitpos) {
    if (!BI || !BI->isConditional())
        return 0;
    
    if (bitpos > NUM_PATH_HEURISTICS) return 0;

    // Get metadata
    MDNode* branchMD = BI->getMetadata("BranchDirection");

    if (!branchMD) return 0;

    MDString* directionMD = dyn_cast<MDString>(branchMD->getOperand(0));
    if (!directionMD) return 0;

    StringRef direction = directionMD->getString();

    if (direction.equals("backward")) {
        // likely a loop back edge and should be taken
        return 1 << bitpos;
    } else if (direction.equals("forward")) {
        return 0;
    }

    return 0;
}
