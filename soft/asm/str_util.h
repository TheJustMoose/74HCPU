#pragma once

#include <cstdint>
#include <string>
#include <vector>

std::string ToUpper(std::string);
std::string NormalizeLine(std::string);
std::string RemoveQuotes(std::string);

bool StrToInt(std::string val, int* pout, std::string* err = nullptr);

bool IsSpace(char);
bool IsDelimeter(char);

std::vector<std::string> Split(std::string);
std::vector<std::string> SplitToCmdParts(std::string);

std::string TrimRight(std::string s);
std::string TrimLeft(std::string s);
std::string Trim(std::string s);

std::string Join(std::vector<std::string> v, char del = '|');
std::string JoinByte(std::vector<uint8_t> v, char del = '|');
std::string JoinInt(std::vector<uint16_t> v, char del = '|');

std::string ToHexString(int value);
