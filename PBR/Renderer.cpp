#include "pch.h"
#include "Renderer.h"

bool Renderer::FrameIsReady() const {
	return frameReady;
}

void Renderer::SetAccumulate(bool state) {
	accumulate = state;
}

void Renderer::SetFrameReady(bool state) {
	frameReady = state;
}