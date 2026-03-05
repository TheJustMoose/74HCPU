#pragma once

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, ntSub, ntMul, ntDiv,
  ntUMinus,   // unary minus
  ntAssign,   // a = b;
  ntUnknown,  // we do not know now what is it
  ntNull
};
