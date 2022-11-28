#version 330 core
layout(location = 0) in vec3 position;
out vec2 fTexCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    fTexCoord = vec2(position.x + 1.0, position.y + 1.0) / 2.0;
}