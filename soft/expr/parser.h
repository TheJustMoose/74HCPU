#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "data_type.h"
#include "node.h"
#include "var.h"

std::unique_ptr<Node> prim();
std::unique_ptr<Node> term();
std::unique_ptr<Node> expr();
std::unique_ptr<Node> assign();
std::unique_ptr<Node> declare();
std::unique_ptr<Node> stmt();
bool stmts(std::vector<std::unique_ptr<Node>>& statements);

size_t getVarCount();
Var getVar(size_t idx);
bool isDeclared(std::string var_name, uint8_t* var_size = 0, DataType* dt = 0);
void printVars();
bool getVar(std::string var_name, Var& var);
