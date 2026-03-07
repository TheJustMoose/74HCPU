#pragma once

enum Token {
  tPlus = 0, tMinus = 1, tMul = 2, tDiv = 3,
  tNum = 4, tName = 5,
  tLBracket = 6, tRBracket = 7, tEqual = 8, tSemicolon = 9,
  tError = 10, tEnd = 11, tNotInitialized = 12
};
