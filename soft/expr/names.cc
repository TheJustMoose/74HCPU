#include "names.h"

#include <map>
#include <string>

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
  {ntAddressOf, "ntAddressOf"},
  {ntDereference, "ntDereference"},
  {ntIncrement, "ntIncrement"},
  {ntDecrement, "ntDecrement"},
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
  {tComma, "tComma"},
  {tAtSign, "tAtSign"},
  {tNumberSign, "tNumberSign"},
  {tIncrement, "tIncrement"},
  {tDecrement, "tDecrement"},
  {tByte, "tByte"},
  {tInt, "tInt"},
  {tNone, "tNone"},
  {tError, "tError"},
  {tEnd, "tEnd"},
  {tNotInitialized, "tNotInitialized"},
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
