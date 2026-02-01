#include "x74HCPUTargetInfo.h"  // Автогенеренный заголовок
#include "llvm/Support/TargetRegistry.h"  // Ключевой заголовок

using namespace llvm;

// ВАЖНО: Эта внешняя (extern "C") функция - ЕДИНСТВЕННАЯ,
// которая должна быть определена в этом файле.
// Её имя ДОЛЖНО соответствовать шаблону: LLVMInitialize<TargetName>TargetInfo
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializex74HCPUTargetInfo() {
  // Регистрируем цель в глобальном реестре LLVM
  RegisterTarget<Triple::x74hcpu> 
    X(getThex74HCPUTarget(),  // Функция, которая возвращает объект цели (объявлена в .h)
      "x74hcpu",              // Короткое имя архитектуры (как в triple)
      "x74HCPU Processor",    // Полное описание
      "x74HCPU"               // Имя для секций .td файлов (обычно совпадает с коротким)
    );
}
