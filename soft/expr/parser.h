#pragma once

#include <memory>
#include <string>
#include <vector>

#include "node.h"

bool program(std::vector<std::unique_ptr<Node>>& statements);
