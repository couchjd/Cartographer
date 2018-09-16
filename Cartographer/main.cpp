#include <GL/glew.h>
#include <glfw3.h>
#include <iostream>

#include "stb_image.h"

const char* vertexShaderSource = {
	"#version 430 core\n"
	"layout (locaton = 0) in vec3 a_Position;\n"
	"void main() {\n"
	"	gl_Position = a_Position;\n"
	"}\0"
};

const char* fragmentShaderSource = {
	"version 430 core\n"
	"void main() {\n"
	"	gl_Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);"
	"}\0"
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width, height, channels;
	int windowWidth, windowHeight;

	std::cout << "LOADING TEXTURE DATA" << std::endl;
	unsigned char* textureData = stbi_load("../res/faerun_no_tags.jpg", &width, &height, &channels, 0);
	std::cout << "FINISHED LOADING TEXTURE DATA" << std::endl;

	if (!textureData) {
		std::cout << "FAILED TO LOAD TEXTURE" << std::endl;
	}

	double scaleFactor = (double)800 / width;
	windowWidth = 800;
	windowHeight = height * scaleFactor;

	std::cout << windowWidth << "\t" << windowHeight << "\t" << scaleFactor << std::endl;

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Cartographer", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewInit();
	glewExperimental = true;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	std::getchar();
	return 0;
}