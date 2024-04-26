#include "opcode_heuristic.h"

int opcodeHeuristic(BranchInst* BI, unsigned int bitpos) {
    if (!BI || !BI->isConditional())
        return 0;
	
	if (bitpos > NUM_PATH_HEURISTICS) return 0;

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

    return 1 << bitpos;  // Default to more likely for all other conditions
}
