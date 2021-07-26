#include "Pass.h"

using namespace llvm;

CreatedBlock FetchBlock(std::vector<CreatedBlock> haystack, BasicBlock* needle){
    for(auto& element : haystack){
        if(needle == std::get<0>(element)) return element;
    }
    return std::make_tuple(nullptr,nullptr,nullptr);
}

BasicBlock* getPrimordial(std::vector<CreatedBlock> haystack, BasicBlock* needle){
    CreatedBlock Fetched = FetchBlock(haystack,needle);
    return std::get<2>(Fetched);
}

void Obfuscate(Module& M){

    srand((unsigned int)time(NULL));

    for(auto& F : M){
        if(!F.size()) continue;

        // List of created Basic blocks
        std::vector<CreatedBlock> CreatedBlocks;

        outs() << "[+] Proceeding on function :" << F.getName() << "\n";

        // Creating the random local variable in the function's entry block
        auto it = F.getEntryBlock().getFirstInsertionPt();
        AllocaInst* Caf = new AllocaInst(Type::getInt32Ty(F.getContext()),0,"VarPtr",&*it);
        ConstantInt* Cons = ConstantInt::get(Type::getInt32Ty(F.getContext()),(unsigned int)rand());
        StoreInst* Stored = new StoreInst(Cons,Caf,&*it);

        for(auto& BB : F){
            LLVMContext& FContext = F.getContext();
            StoreInst* StoredLocal = nullptr;

            // Discard Basic blocks created by the pass
            if(std::get<0>(FetchBlock(CreatedBlocks,&BB))) continue;
            Instruction* Terminator = BB.getTerminator();

            if(&BB != &F.getEntryBlock()){
                Cons = ConstantInt::get(Type::getInt32Ty(FContext),(unsigned int)rand());
                StoredLocal = new StoreInst(Cons,Caf,&*BB.getFirstInsertionPt());
            }

            // Registering each intsruction
            std::vector<Instruction*> InsList;
            std::vector<PHINode*> PHIList;

            for(auto& I : BB){
                LLVMContext& BBContext = BB.getContext();

                /* Discarded instructions:
                    - Allocation of the random variable
                    - Storage of the random variable
                    - First storage of random variable
                    - Any other storage of random variable
                    - landing pads instruction
                    - Phi nodes
                */

                if(&I == Caf || &I == Stored || &I == StoredLocal || isa<LandingPadInst>(&I)) continue;
                else if(isa<PHINode>(&I)){
                    PHIList.push_back(cast<PHINode>(&I));
                    continue;
                }
                InsList.push_back(&I);
            }

            // Creating Dispacher basic block
            BasicBlock* DispacherBB = BasicBlock::Create(FContext,"Dispacher",&F);
            CreatedBlocks.push_back(std::make_tuple(DispacherBB,nullptr,&BB));

            // Creating new basic blocks for each instructions
            for(auto& InstructionPtr : InsList){

                BasicBlock* NewBB = BasicBlock::Create(FContext,"BB",&F);
                IRBuilder<> NewBB_builder(NewBB);
                InstructionPtr->removeFromParent();
                NewBB_builder.Insert(InstructionPtr);
                CreatedBlocks.push_back(std::make_tuple(NewBB,DispacherBB,&BB));

                // JMP back to the dispacher once Basic Block is executed
                if(InstructionPtr != Terminator){
                    BranchInst* BackToDispacher = BranchInst::Create(DispacherBB,NewBB);
                }
            }
            // JMP from entry to dispacher
            BranchInst* GoToDispacher = BranchInst::Create(DispacherBB,&BB);

            // Creating switch
            CreateNewSwitch(FContext,Caf,DispacherBB,CreatedBlocks,InsList,Terminator,Cons);

            // Resolving PHInodes
            if(PHIList.size()) FixupPhiNodes(PHIList, CreatedBlocks);
        }
        outs() << "Done!\n";
    }
}

void FixupPhiNodes(std::vector<PHINode*> PHIList,std::vector<CreatedBlock> CreatedBlocks){
    for(PHINode* node : PHIList){
        for(int i = 0 ; i < node->getNumIncomingValues() ; i ++){
            BasicBlock* Incoming = node->getIncomingBlock(i);
            for(BasicBlock* BBPtr : predecessors(node->getParent())){
                if(getPrimordial(CreatedBlocks,BBPtr) == Incoming){
                    node->replaceIncomingBlockWith(Incoming,BBPtr);
                }
            }
        }
    }
}

void CreateNewSwitch(LLVMContext& ctx,AllocaInst* Var, BasicBlock* DispacherBB, std::vector<CreatedBlock> CreatedBlocks, std::vector<Instruction*> InsList,Instruction* Terminator, ConstantInt* Cons){
    LoadInst* Loaded = new LoadInst(Type::getInt32Ty(ctx),Var,"Var",DispacherBB);
    SwitchInst* Dispacher = SwitchInst::Create(Loaded,std::get<0>(CreatedBlocks.back()),InsList.size(),DispacherBB);
    ConstantInt* Next = ConstantInt::get(Type::getInt32Ty(ctx),Cons->getZExtValue());

    for(auto& InstructionPtr : InsList){
        
        Dispacher->addCase(Next,InstructionPtr->getParent());
        unsigned int Ran = (unsigned int)rand();

        if(InstructionPtr != Terminator){
            uint64_t num = Next->getZExtValue() ^ Ran;
            Next = ConstantInt::get(Type::getInt32Ty(ctx),num);
            Value* ret = BinaryOperator::CreateXor(Loaded,ConstantInt::get(Type::getInt32Ty(ctx),Ran),"ret",InstructionPtr->getParent()->getTerminator());
            StoreInst* StorePtr = new StoreInst(ret,Var,InstructionPtr->getParent()->getTerminator());
        }
    }
}
