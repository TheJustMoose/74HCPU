#pragma once

enum Token {
  tPlus = 0, tMinus, tMul, tDiv,
  tNum, tName,
  tComma, tSemicolon, tAtSign, tNumberSign,
  tLBracket, tRBracket, tAssign,
  tLess, tGreater, tLessOrEqual, tGreaterOrEqual,
  tEqual, tNotEqual,
  tIncrement, tDecrement,
  tBool, tByte, tInt,
  tInversion,
  tIf, tElse,
  tNone,
  tError, tEnd, tNotInitialized
};
