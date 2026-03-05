#pragma once

enum Token {
  tPlus, tMinus, tMul, tDiv,
  tNum, tName,
  tLBracket, tRBracket, tEqual, tSemicolon,
  tError, tEnd
};
