#pragma once

#include "node_type.h"
#include "token.h"

#include <string>

std::string GetNodeTypeName(NodeType);
std::string GetTokenName(Token);
NodeType Token2NodeType(Token t);
