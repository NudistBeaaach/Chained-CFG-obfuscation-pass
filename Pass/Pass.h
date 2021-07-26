#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/CFG.h"
#include <vector>
#include <tuple>

using namespace llvm;

  typedef std::tuple<BasicBlock*, BasicBlock*, BasicBlock*> CreatedBlock;

  void FixupPhiNodes(std::vector<PHINode*> PHIList, std::vector<CreatedBlock> CreatedBlocks);

  void CreateNewSwitch(LLVMContext& ctx,AllocaInst* Var, BasicBlock* DispacherBB, std::vector<CreatedBlock> CreatedBlocks, std::vector<Instruction*> InsList, Instruction* Terminator, ConstantInt* Cons);

  void Obfuscate(Module& M);

  CreatedBlock FetchBlock(std::vector<CreatedBlock> haystack, BasicBlock* needle);

  BasicBlock* getPrimordial(std::vector<CreatedBlock> haystack, BasicBlock* needle);

    // Pass implementation
    struct PassTheFishe : PassInfoMixin<PassTheFishe>{
        PreservedAnalyses run(Module& M, ModuleAnalysisManager &){
            Obfuscate(M);
            return PreservedAnalyses::all();
        }
    };

// Pass registration
llvm::PassPluginLibraryInfo FishePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PassTheFishe", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "Pass-Fishe") {
                    MPM.addPass(PassTheFishe());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return FishePluginInfo();
}
