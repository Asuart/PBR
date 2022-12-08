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

struct PackedBVHNode {
	glm::vec4 bbMin, bbMax;
	int32_t leftIndex;
	int32_t rightIndex;
	int32_t leafIndex;
	int32_t isLeaf;
};

struct PackedBVHLeaf {
	int32_t triangleIndexes[MeshBVH::maxTriangles];
};

GPURayTracer::GPURayTracer(int32_t width, int32_t height, bool accumulate)
	: RayTracer(width, height, 6), trianglesCount(0), trianglesBufferData(nullptr) {
	film = new Film(width, height);
	film->SetupRenderQuad();

	rayProgram = LoadComputeShader("C:/Users/User/Desktop/PBR/PBR/RayComputeShader.glsl");
}

GPURayTracer::~GPURayTracer() {
	delete film;
	delete[] trianglesBufferData;
}

void GPURayTracer::SetScene(Scene* s) {
	scene = s;
	
	std::vector<TriangleAligned> encodedTriangles;

	auto objects = scene->GetObjectWithMeshes();
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

	trianglesCount = encodedTriangles.size();

	glGenBuffers(1, &trianglesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesCount * sizeof(TriangleAligned), &encodedTriangles[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, trianglesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &hitResultsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitResultsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, film->width * film->height * sizeof(HitResultAligned), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, hitResultsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPURayTracer::StartRender() {
	Time::MeasureStart("Time to sample");
	glUseProgram(rayProgram);

	int32_t patchSize = 100;
	int32_t patchesCount = trianglesCount / patchSize + 1;

	glUniform1i(glGetUniformLocation(rayProgram, "TRIANGLES_COUNT"), trianglesCount);
	glUniform1i(glGetUniformLocation(rayProgram, "PATCH_SIZE"), patchSize);
	glUniform1i(glGetUniformLocation(rayProgram, "MAX_BOUNCES"), maxBounces);

	for (int32_t i = 0; i < patchesCount; i++) {
		glUniform1i(glGetUniformLocation(rayProgram, "PATCH_INDEX"), i);
		glDispatchCompute((GLuint)film->width, (GLuint)film->height, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glFinish();
	}

	Time::MeasureEnd("Time to sample");
}

void GPURayTracer::EndRender() {

}