#pragma once

#include <string>
#include <vector>

class Node;

Node* prim();
Node* term();
Node* expr();
Node* assign();
Node* declare();
Node* stmt();
bool stmts(std::vector<Node*>& statements);

bool isDeclared(std::string var_name, int* var_size = 0);
