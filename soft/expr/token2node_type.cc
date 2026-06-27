#include "token2node_type.h"

NodeType Token2NodeType(Token t) {
  switch (t) {
    case tPlus: return ntSum;
    case tMinus: return ntSub;
    case tMul: return ntMul;
    case tDiv: return ntDiv;
    case tAssign: return ntAssign;
    case tLess: return ntLess;
    case tGreater: return ntGreater;
    case tLessOrEqual: return ntLessOrEqual;
    case tGreaterOrEqual: return ntGreaterOrEqual;
    default: return ntUnknown;
  }
}
