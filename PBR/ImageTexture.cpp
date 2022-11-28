#include "pch.h"
#include "ImageTexture.h"

ImageTexture::ImageTexture()
	: data(nullptr), width(0), height(0) 
{

}

ImageTexture::ImageTexture(const char* filePath) 
	: ImageTexture()
{
	return;
	int32_t components_per_pixel = 3;
	uint8_t* d;// = stbi_load(filePath, &width, &height, &components_per_pixel, components_per_pixel);

	if (!d) {
		std::cerr << "ERROR: Could not load texture image file '" << filePath << "'.\n";
		width = height = 0;
	}

	data = new glm::vec3[width * height];
	for (int32_t i = 0; i < width * height; i++) {
		data[i] = glm::vec3(d[i * components_per_pixel] / 255.0f, d[i * components_per_pixel] / 255.0f, d[i * components_per_pixel + 2] / 255.0f);
	}

	delete[] d;
}

ImageTexture::~ImageTexture() {
	delete[] data;
}

glm::vec3 ImageTexture::Sample(const glm::vec2& uv, const glm::vec3& p, float time) const {
	if (data == nullptr)
		return glm::vec3(0, 1, 1);

	float u = glm::clamp(uv.x, 0.0f, 1.0f);
	float v = 1.0f - glm::clamp(uv.y, 0.0f, 1.0f);  // Flip V to image coordinates

	int32_t i = static_cast<int32_t>(u * width);
	int32_t j = static_cast<int32_t>(v * height);

	// Clamp integer mapping, since actual coordinates should be less than 1.0
	if (i >= width)  i = width - 1;
	if (j >= height) j = height - 1;

	return data[j * width + i];
}