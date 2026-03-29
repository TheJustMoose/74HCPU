git submodule update
md bin
cd bin
del "..\..\Release\simple_expr.exe"
del "..\..\Release\simple_expr_unit_tests.exe"

cmake ..
cmake --build . --config Release
REM cmake --build . --config Debug
