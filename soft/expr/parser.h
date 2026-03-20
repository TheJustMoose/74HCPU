#pragma once

#include <cstdint>
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

bool isDeclared(std::string var_name, uint8_t* var_size = 0);
void printVars();
