#pragma once
#include <Windows.h>

__declspec(dllexport) int Sum(int a, int b);
__declspec(dllexport) int Sub(int a, int b);
__declspec(dllexport) void Hacked();
__declspec(dllexport) void Replace(const char* data, const char* replacement);