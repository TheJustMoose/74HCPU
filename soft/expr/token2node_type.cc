#include "token2node_type.h"

NodeType Token2NodeType(Token t) {
  switch (t) {
    case tPlus: return ntSum;
    case tMinus: return ntSub;
    case tMul: return ntMul;
    case tDiv: return ntDiv;
    case tEqual: return ntAssign;
    default: return ntUnknown;
  }
}
