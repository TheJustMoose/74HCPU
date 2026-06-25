#pragma once

enum Token {
  tPlus = 0, tMinus, tMul, tDiv,
  tNum, tName,
  tComma, tSemicolon, tAtSign, tNumberSign,
  tLBracket, tRBracket, tEqual,
  tLess, tGreater, tLessOrEqual, tGreaterOrEqual,
  tIncrement, tDecrement,
  tByte, tInt,
  tNone,
  tError, tEnd, tNotInitialized
};
