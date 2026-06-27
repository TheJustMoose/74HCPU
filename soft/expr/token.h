#pragma once

enum Token {
  tPlus = 0, tMinus, tMul, tDiv,
  tNum, tName,
  tComma, tSemicolon, tAtSign, tNumberSign,
  tLBracket, tRBracket, tAssign,
  tLess, tGreater, tLessOrEqual, tGreaterOrEqual,
  // tEqual, tNotEqual,
  tIncrement, tDecrement,
  tByte, tInt,
  tNone,
  tError, tEnd, tNotInitialized
};
