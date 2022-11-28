#pragma once
#include "pch.h"

std::string ReadFile(const char* filePath);
GLuint LoadShader(const char* vertex_path, const char* fragment_path);
GLuint LoadComputeShader(const char* compute_path);