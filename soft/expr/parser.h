#pragma once

#include <vector>

class Node;

Node* prim();
Node* term();
Node* expr();
Node* assign();
Node* declare();
Node* stmt();
bool stmts(std::vector<Node*>& statements);
