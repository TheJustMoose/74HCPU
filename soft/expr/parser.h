#pragma once

#include <vector>

class Node;

Node* prim();
Node* term();
Node* expr();
Node* assign();
bool stmt(std::vector<Node*>& statements);
