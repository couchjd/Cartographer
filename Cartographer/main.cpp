#include <GL/glew.h>
#include <glfw3.h>
#include "stb_image.h"
#include "shader_s.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;
float zoomValue = 0.5f;

glm::fvec2 offset = { 0.0f, 0.0f };

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glewInit();
	glewExperimental = true;

	Shader ourShader("../res/shaders/vertexShader.glsl", "../res/shaders/fragmentShader.glsl");

	float vertices[] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int texture1, texture2;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	std::cout << "Loading IMAGE 1..." << std::endl;
	unsigned char *data = stbi_load("../res/textures/faerun_no_tags.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		std::cout << "Done loading IMAGE 1. Generating TEXTURE 1..." << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		std::cout << "Done generating TEXTURE 1. Generating MIPMAPS 1..." << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Done generating MIPMAPS 1." << std::endl;
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout << "Loading IMAGE 2..." << std::endl;
	data = stbi_load("../res/textures/faerun_tags.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		std::cout << "Done loading IMAGE 2. Generating TEXTURE 2..." << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		std::cout << "Done generating TEXTURE 2. Generating MIPMAPS 2..." << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Done generating MIPMAPS 2." << std::endl;
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	// or set it via the texture class
	ourShader.setInt("texture2", 1);


	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glm::mat4 transform(1);

		transform = glm::translate(transform, glm::vec3(offset.x, offset.y, 0.0f));
		transform = glm::scale(transform, glm::vec3(zoomValue, zoomValue, 0.0f));
		ourShader.setFloat("mixValue", mixValue);

		ourShader.use();
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		mixValue = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		zoomValue -= 0.05f;
		if (zoomValue <= 0.25f)
			zoomValue = 0.25f;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		zoomValue += 0.05f;
		if (zoomValue >= 20.0f)
			zoomValue = 20.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		offset.y -= 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		offset.y += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		offset.x += 0.005f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		offset.x -= 0.005f;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}