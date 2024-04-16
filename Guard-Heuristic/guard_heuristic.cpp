#include <iostream>
#include <cassert>
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

int guardHeuristic(BranchInst* BI, unsigned int bitpos) {
    // Ensure the branch is conditional
    if (!BI || !BI->isConditional())
        return 0;

    if (bitpos > NUM_PATH_HEURISTICS) return 0;

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
                            return (1 << bitpos);
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
