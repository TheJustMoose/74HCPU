# Macro for LLVM targets
add_llvm_component_group(x74HCPU)

# Static library for backend
add_llvm_target(x74HCPUCodeGen x74HCPUTargetMachine.cpp)
