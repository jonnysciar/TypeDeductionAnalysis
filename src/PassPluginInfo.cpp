#include "TypeDeductionAnalysis.hpp"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

#if LLVM_VERSION_MAJOR >= 22
#include "llvm/Plugins/PassPlugin.h"
#else
#include "llvm/Passes/PassPlugin.h"
#endif

using namespace llvm;

// Tiny driver transform pass that just runs the analysis once
struct TDARunnerPass : PassInfoMixin<TDARunnerPass> {
  PreservedAnalyses run(Module& M, ModuleAnalysisManager& MAM) {
    (void) MAM.getResult<tda::TypeDeductionAnalysis>(M);
    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "TypeDeductionAnalysis", "1.0", [](PassBuilder& passBuilder) {
            passBuilder.registerAnalysisRegistrationCallback([](ModuleAnalysisManager& moduleAnalysisManager) {
              moduleAnalysisManager.registerPass([&] { return tda::TypeDeductionAnalysis(); });
            });

            passBuilder.registerPipelineParsingCallback(
              [](StringRef name, ModulePassManager& modulePassManager, ArrayRef<PassBuilder::PipelineElement>) {
                if (name == "tda") {
                  modulePassManager.addPass(TDARunnerPass());
                  return true;
                }
                return false;
              });
          }};
}
