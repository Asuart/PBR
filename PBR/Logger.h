#pragma once
#include "pch.h"

class Logger {
	Logger() = delete;
public:
	static void Error(const std::string& message);
};

