#include "pch.h"
#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() {
	frameReady = true;
}

DefaultRenderer::~DefaultRenderer() {

}

void DefaultRenderer::Reset() {}

void DefaultRenderer::SetScene(const Scene& scene) {
	FreeGPU();
	activeScene = &scene;
	const std::vector<SceneObject*>& obj = scene.GetObjectWithMeshes();
	for (int32_t i = 0; i < obj.size(); i++) {
		const Mesh& mesh = obj[i]->GetMesh();
		ObjectOnGPU entry;
		entry.trianglesCount = mesh.indices.size();
		glGenVertexArrays(1, &entry.vao);
		glBindVertexArray(entry.vao);
		glGenBuffers(1, &entry.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, entry.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(2, MAX_BONES_PER_VERTEX, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, MAX_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);
		objects.push_back(entry);
	}
}

void DefaultRenderer::SetCamera(const Camera& camera) {
	activeCamera = &camera;
}

void DefaultRenderer::Resize(int32_t _width, int32_t _height) {
	width = _width;
	height = _height;
}

void DefaultRenderer::RenderSample() {}

void DefaultRenderer::StartRender() {
	defaultShaderProgram = LoadShader("C:/Users/User/Desktop/PBR/PBR/DefaultVertexShader.glsl", "C:/Users/User/Desktop/PBR/PBR/DefaultFragmentShader.glsl");
}

void DefaultRenderer::EndRender() {

}

void DefaultRenderer::DrawFrame() {
	glUseProgram(defaultShaderProgram);
	GLuint viewLoc = glGetUniformLocation(defaultShaderProgram, "mView");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)&activeCamera->GetViewMatrix());
	GLuint projectionLoc = glGetUniformLocation(defaultShaderProgram, "mProjection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &activeCamera->GetProjectionMatrix()[0][0]);

	for (int32_t i = 0; i < objects.size(); i++) {
		glBindVertexArray(objects[i].vao);
		glDrawArrays(GL_TRIANGLES, 0, objects[i].trianglesCount);
		glBindVertexArray(0);
	}

	sample++;
}

float DefaultRenderer::GetProgress() const {
	return 1.0f;
}

void DefaultRenderer::FreeGPU() {

}