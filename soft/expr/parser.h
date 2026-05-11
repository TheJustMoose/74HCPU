#pragma once

#include <memory>
#include <string>
#include <vector>

#include "node.h"

std::unique_ptr<Node> prim();
std::unique_ptr<Node> term();
std::unique_ptr<Node> expr();
std::unique_ptr<Node> assign();
std::unique_ptr<Node> declare();
std::unique_ptr<Node> stmt();
bool stmts(std::vector<std::unique_ptr<Node>>& statements);
