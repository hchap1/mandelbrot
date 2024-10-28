#version 400 core

layout (location = 0) in vec2 aPos;

out float x_pos;
out float y_pos;


void main() {
	x_pos = aPos.x * (1920.0 / 1080.0);
	y_pos = aPos.y;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
