#version 330 core

in vec3 fNormal;

out vec4 color;

void main()
{
	float cos = abs(dot(fNormal, vec3(gl_FragCoord.x / 600.0, gl_FragCoord.y / 600.0, 1)));


	color = vec4(cos, cos, cos, 1.0) * 0.8 + vec4(1.0, 1.0, 1.0, 1.0) * 0.2;
}