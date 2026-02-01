set(LLVM_TARGET_DEFINITIONS x74HCPU.td)

add_subdirectory(TargetInfo)

# Static library for backend
add_llvm_target(x74HCPUCodeGen x74HCPUTargetMachine.cpp)  # will add prefix LLVM to x74HCPUCodeGen

if(TARGET LLVMx74HCPUCodeGen)
    message(STATUS "Okey, target found!")
else()
    message(STATUS "Oops :( No target")
endif()

# Link libraries
target_link_libraries(LLVMx74HCPUCodeGen PRIVATE LLVMSupport)
