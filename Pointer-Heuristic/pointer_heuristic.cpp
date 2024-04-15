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
                    return 1;  // Encode as '1' indicating more likely
                default:
                    // For other pointer comparisons, default to a neutral expectation
                    return 0;
            }
        }
    }
    return 0;
}
