#ifndef LLVM_LIB_TARGET_X74HCPU_X74HCPU_H
#define LLVM_LIB_TARGET_X74HCPU_X74HCPU_H

//#include "MCTargetDesc/x74HCPUMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class x74HCPUTargetMachine;
  class FunctionPass;

  // Эта функция будет объявлена в x74HCPUTargetMachine.cpp
  // Она создаёт проходы для генерации кода под 74HCPU.
  FunctionPass *create74HCPUISelDag(x74HCPUTargetMachine &TM);
}

#endif
