#pragma once

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, ntSub, ntMul, ntDiv,
  ntUMinus,      // unary minus
  ntAssign,      // a = b;
  ntAddressOf,   // #
  ntDereference, // @
  ntIncrement,   // ++
  ntDecrement,   // --
  ntLess,        // <
  ntGreater,     // >
  ntLessOrEqual, // <=
  ntGreaterOrEqual, // >=
  ntUnknown,     // we do not know now what is it
  ntVarDecl,     // variable declaration node
  ntNull
};
