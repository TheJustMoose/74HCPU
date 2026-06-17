git submodule update
md build
cd build
del "..\..\Release\simple_expr.exe"
del "..\..\Release\simple_expr_unit_tests.exe"

cmake ..
cmake --build . --config Release
REM cmake --build . --config Debug
