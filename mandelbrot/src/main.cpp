#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <CUSTOM/renderer.h>
#include <CUSTOM/shader.h>

int main() {
	Renderer renderer = Renderer(800, 800, "MandelBrot");
	RenderLayer testTriangle({ 2 }, "complex");

	float vertices[] = {
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f
	};

	float x_min = -2.0;
	float x_max = 1.0;
	float y_min = -2.0;
	float y_max = 2.0;
	
	float x_mag = x_max - x_min;
	float y_mag = y_max - x_min;

	float zoom = 1.0;
	float center_x = 0.286017;
	float center_y = 0.0115375;

	float offset_a = 0.0;
	float offset_b = 0.0;

	float order = 500.0f;
	
	testTriangle.setVertices(vertices, 2, 6, GL_DYNAMIC_DRAW);
	testTriangle.setFloat("x_min", x_min);
	testTriangle.setFloat("x_mag", x_mag);
	testTriangle.setFloat("y_min", y_min);
	testTriangle.setFloat("y_mag", y_mag);
	testTriangle.setFloat("zoom", zoom);
	testTriangle.setFloat("center_x", center_x);
	testTriangle.setFloat("center_y", center_y);
	testTriangle.setFloat("offset_a", offset_a);
	testTriangle.setFloat("offset_b", offset_b);
	testTriangle.setFloat("o", order);

	glfwSwapInterval(1);

	while (renderer.isRunning()) {
		double dt = renderer.getDeltaTime();
		testTriangle.setFloat("zoom", zoom);
		testTriangle.setFloat("center_x", center_x);
		testTriangle.setFloat("center_y", center_y);
		testTriangle.setFloat("offset_a", offset_a);
		testTriangle.setFloat("o", order);
		testTriangle.setFloat("offset_b", offset_b);
		testTriangle.draw(2);
		if (renderer.getKeyDown(GLFW_KEY_SPACE)) {
			zoom *= powf(1.05f, dt * 60.0f);
		}
		if (renderer.getKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			zoom /= powf(1.05f, dt * 60.0f);
		}
		if (renderer.getKeyDown(GLFW_KEY_A)) {
			center_x -= dt / zoom;
		}
		if (renderer.getKeyDown(GLFW_KEY_D)) {
			center_x += dt / zoom;
		}
		if (renderer.getKeyDown(GLFW_KEY_W)) {
			center_y += dt / zoom;
		}
		if (renderer.getKeyDown(GLFW_KEY_S)) {
			center_y -= dt / zoom;
		}
		if (renderer.mouseWasJustClicked()) {
			std::cout << "Position: (" << center_x << ", " << center_y << "), Zoom: " << zoom << ", C: (" << offset_a << ", " << offset_b << ")" << std::endl;
		}
		if (renderer.getKeyDown(GLFW_KEY_UP)) {
			offset_b += dt;
		}
		if (renderer.getKeyDown(GLFW_KEY_DOWN)) {
			offset_b -= dt;
		}
		if (renderer.getKeyDown(GLFW_KEY_RIGHT)) {
			offset_a += dt;
		}
		if (renderer.getKeyDown(GLFW_KEY_LEFT)) {
			offset_a -= dt;
		}
		if (renderer.getKeyDown(GLFW_KEY_R)) {
			offset_a = 0.0;
			offset_b = 0.0;
		}
		if (renderer.getKeyDown(GLFW_KEY_I)) {
			order += dt * 100.0f;
		}
		if (renderer.getKeyDown(GLFW_KEY_O)) {
			order -= dt * 100.0f;
		}
		renderer.updateDisplay();
	}
	return 1;
}