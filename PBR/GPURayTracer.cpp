#include "pch.h"
#include "GPURayTracer.h"

struct TriangleAligned {
	glm::vec4 p1, p2, p3;    // Vertex coordinates
	glm::vec4 n1, n2, n3;    // Vertex normal
	glm::vec4 emissive;      // Self luminous parameters
	glm::vec4 baseColor;     // colour
	glm::vec4 param1;        // (subsurface, metallic, specular)
	glm::vec4 param2;        // (specularTint, roughness, anisotropic)
	glm::vec4 param3;        // (sheen, sheenTint, clearcoat)
	glm::vec4 param4;        // (clearcoatGloss, IOR, transmission)
};

struct HitResultAligned {
	glm::vec4 position; // pos, distance
	glm::vec4 normal; // normal
};


GPURayTracer::GPURayTracer(int32_t width, int32_t height, bool accumulate)
	: trianglesCount(0), trianglesBufferData(nullptr)
{
	Resize(width, height);
	SetAccumulate(accumulate);
	SetupRenderQuad();

	rayProgram = LoadComputeShader("C:/Users/User/Desktop/PBR/PBR/RayComputeShader.glsl");
}

GPURayTracer::~GPURayTracer() {
	delete[] trianglesBufferData;
}

void GPURayTracer::Reset() {
	sample = 0;
}

void GPURayTracer::SetScene(const Scene& scene) {
	activeScene = &scene;
	
	std::vector<TriangleAligned> encodedTriangles;

	auto objects = activeScene->GetObjectWithMeshes();
	for (int32_t i = 0; i < objects.size(); i++) {
		Mesh mesh = objects[i]->GetMesh();
		for (int32_t i = 0; i < mesh.indices.size(); i += 3) {
			TriangleAligned tri;
			tri.p1 = glm::vec4(mesh.vertices[mesh.indices[i]].p, 1.0);
			tri.p2 = glm::vec4(mesh.vertices[mesh.indices[i + 1]].p, 1.0);
			tri.p3 = glm::vec4(mesh.vertices[mesh.indices[i + 2]].p, 1.0);
			tri.n1 = glm::vec4(mesh.vertices[mesh.indices[i]].n, 1.0);
			tri.n2 = glm::vec4(mesh.vertices[mesh.indices[i + 1]].n, 1.0);
			tri.n3 = glm::vec4(mesh.vertices[mesh.indices[i + 2]].n, 1.0);
			tri.baseColor = glm::vec4(1.0, 1.0, 0.0, 1);
			encodedTriangles.push_back(tri);
		}
	}
	


	/*
	TriangleAligned t;
	t.p1 = glm::vec4(0, 0, 0, 1);
	t.p2 = glm::vec4(0, 1, 0, 1);
	t.p3 = glm::vec4(1, 0, 0, 1);
	t.n1 = glm::vec4(0, 0, 1, 1);
	t.n2 = glm::vec4(0, 0, 1, 1);
	t.n3 = glm::vec4(0, 0, 1, 1);
	t.baseColor = glm::vec4(1.0, 1.0, 0.0, 1);

	encodedTriangles.push_back(t);
		*/
	trianglesCount = encodedTriangles.size();

	glGenBuffers(1, &trianglesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesCount * sizeof(TriangleAligned), &encodedTriangles[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, trianglesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &hitResultsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitResultsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * sizeof(HitResultAligned), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, hitResultsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPURayTracer::SetCamera(const Camera& camera) {
	activeCamera = &camera;
}

void GPURayTracer::Resize(int32_t _width, int32_t _height) {
	width = _width;
	height = _height;
	pixelSize = glm::vec2(1.0f / width, 1.0f / height);
}

void GPURayTracer::RenderSample() {
	Time::MeasureStart("Time to sample");
	glUseProgram(rayProgram);

	int32_t patchSize = 100;
	int32_t patchesCount = trianglesCount / patchSize + 1;

	glUniform1i(glGetUniformLocation(rayProgram, "TRIANGLES_COUNT"), trianglesCount);
	glUniform1i(glGetUniformLocation(rayProgram, "PATCH_SIZE"), patchSize);
	glUniform1i(glGetUniformLocation(rayProgram, "MAX_BOUNCES"), 5);

	for (int32_t i = 0; i < patchesCount; i++) {
		glUniform1i(glGetUniformLocation(rayProgram, "PATCH_INDEX"), i);
		glDispatchCompute((GLuint)width, (GLuint)height, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glFinish();
	}

	sample++;

	frameReady = true;

	Time::MeasureEnd("Time to sample");
}

void GPURayTracer::DrawFrame() {
	glUseProgram(quadProgram);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	frameReady = false;
}


void GPURayTracer::StartRender() {

}

void GPURayTracer::EndRender() {

}

float GPURayTracer::GetProgress() const {
	return 1.0f;
}