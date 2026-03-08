#include <map>
#include <string>

#include "names.h"

using namespace std;

map<NodeType, string> NodeName {
  {ntName, "ntName"},
  {ntNum, "ntNum"},
  {ntSum, "ntSum"},
  {ntSub, "ntSub"},
  {ntMul, "ntMul"},
  {ntDiv, "ntDiv"},
  {ntUMinus, "ntUMinus"},
  {ntAssign, "ntAssign"},
  {ntUnknown, "ntUnknown"},
  {ntVarDecl, "ntVarDecl"},
  {ntNull, "ntNull"},
};

map<Token, string> TokenName {
  {tPlus, "tPlus"},
  {tMinus, "tMinus"},
  {tMul, "tMul"},
  {tDiv, "tDiv"},
  {tNum, "tNum"},
  {tName, "tName"},
  {tLBracket, "tLBracket"},
  {tRBracket, "tRBracket"},
  {tEqual, "tEqual"},
  {tSemicolon, "tSemicolon"},
  {tEnd, "tEnd"},
  {tInt, "tInt"},
};

string GetNodeTypeName(NodeType nt) {
  if (NodeName.find(nt) != NodeName.end())
    return NodeName[nt];
  else
    return "Unknown NodeType";
}

string GetTokenName(Token t) {
  if (TokenName.find(t) != TokenName.end())
    return TokenName[t];
  else
    return "Unknown Token";
}

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
