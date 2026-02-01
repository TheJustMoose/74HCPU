#include "x74HCPU.h"
//#include "x74HCPUSubtarget.h"
#include "TargetInfo/x74HCPUTargetInfo.h"
//#include "llvm/CodeGen/TargetPassConfig.h"
//#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializex74HCPUTarget() {
  // Регистрируем нашу архитектуру в системе LLVM.
  // "74HCPU" — это тройная архитектура (как в -mtriple=74HCPU-...)
  // x74HCPUTargetMachine::create — фабричная функция.
  RegisterTargetMachine<x74HCPUTargetMachine> X(getThe74HCPUTarget());
}

// Конструктор. Задаём Data Layout — описание базовых свойств архитектуры.
// "e" — little-endian, "m:e" — ELF формат, "p:32:32" — 32-битные указатели,
// "i64:64" — 64-битные целые выровнены по 64 бита.
x74HCPUTargetMachine::x74HCPUTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, "e-m:e-p:32:32-i64:64-n32", TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM, CM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<C74HCPUELFTargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

// Создаём конфигурацию проходов (pass manager) для нашей цели.
namespace {
class x74HCPUPassConfig : public TargetPassConfig {
public:
  x74HCPUPassConfig(x74HCPUTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}
  // Пока мы просто указываем, что на этапе ISel (выбора инструкций)
  // должен использоваться наш создатель DAG.
  bool addInstSelector() override {
    addPass(create74HCPUISelDag(getTM<x74HCPUTargetMachine>()));
    return false;
  }
};
} // namespace

TargetPassConfig *x74HCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new x74HCPUPassConfig(*this, PM);
}
