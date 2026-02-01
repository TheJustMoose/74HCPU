// 1. Включаем общие определения LLVM для TableGen
include "llvm/Target/Target.td"

// 2. Объявляем фиктивный класс InstructionSet, если его ещё нет.
//    Это ЗАГЛУШКА - чтобы Target мог ссылаться на x74HCPUInstrInfo.
class x74HCPUInstrInfo : InstructionSet;

// 3. Объявляем ИМЯ АРХИТЕКТУРЫ для использования в тройках (triple).
//    Это должно совпадать с именем, которое вы использовали в RegisterTarget.
def x74HCPU : Target {
  let InstructionSet = x74HCPUInstrInfo;
}

// 4. ДОБАВЛЯЕМ ЭТУ СТРОКУ - объявление архитектуры для Triple.
//    Без этого LLVM не узнает архитектуру "x74hpu" в командах.
def Arch_x74hcpu : Architecture<"x74hcpu">;

// 5. Объявляем ПРОЦЕССОРЫ (поддерживаемые модели чипов).
//    Пока можно создать один "generic".
def : ProcessorModel<"x74HCPU", x74HCPUModel, [/* Список будущих фич */]>;

// 6. Определяем КЛАСС МОДЕЛИ процессора. Это абстракция для группы CPU.
def x74HCPUModel : ProcessorItineraries<[/* Можно оставить пустым */]>;
