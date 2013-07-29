#pragma once

#include <string>
#include <vector>

std::wstring GetCurrentDir();
std::vector<std::wstring> GetDirFiles(std::wstring &dir);
