#include "pch.h"
#include "Logger.h"

void Logger::Error(const std::string& message) {
	std::cout << "ERROR:: " << message << "\n";
}