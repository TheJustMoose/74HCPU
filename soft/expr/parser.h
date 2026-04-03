#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "var.h"

class Node;

Node* prim();
Node* term();
Node* expr();
Node* assign();
Node* declare();
Node* stmt();
bool stmts(std::vector<Node*>& statements);

size_t getVarCount();
Var getVar(size_t idx);
bool isDeclared(std::string var_name, uint8_t* var_size = 0);
void printVars();
bool getVar(std::string var_name, Var& var);
