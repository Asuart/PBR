#version 330 core
in vec2 fTexCoord;

uniform sampler2D ourTexture;

out vec4 color;

void main() {
    color = vec4(fTexCoord.x, fTexCoord.y, 1.0, 1.0);
    color = texture(ourTexture, fTexCoord);
}