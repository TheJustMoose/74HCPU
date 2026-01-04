del unit_tests.exe
"C:\Program Files\LLVM\bin\clang.exe" -std=c++17 -I doctest\doctest ^
 unit_tests.cc const.cc lexer.cc node.cc num.cc parser.cc real.cc stmt.cc tag.cc temp.cc token.cc -o unit_tests.exe >err.txt 2>&1
