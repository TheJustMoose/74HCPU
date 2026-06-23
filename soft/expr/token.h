#pragma once

enum Token {
  tPlus = 0, tMinus = 1, tMul = 2, tDiv = 3,
  tNum = 4, tName = 5,
  tLBracket = 6, tRBracket = 7, tEqual = 8, tSemicolon = 9,
  tComma = 10, tAtSign = 11, tNumberSign = 12,
  tIncrement = 13, tDecrement = 14,
  tByte = 15, tInt = 16,
  tNone = 17,
  tError = 18, tEnd = 19, tNotInitialized = 20
};
