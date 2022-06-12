#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h"
#include "shader_s.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow*, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
void loadTexture(const char* filename, unsigned int& texture1, int& width, int& height);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const char* TITLE = "Cartographer\0";

glm::vec3 viewVec(0.0f, 0.0f, -3.0f);
float mixValue = 0.0f;

int main() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewInit();
	glewExperimental = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	const char* glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	Shader ourShader("../res/shaders/vertexShader.glsl", "../res/shaders/fragmentShader.glsl");

	unsigned int texture1, texture2;
	int width, height;

	loadTexture("../res/textures/faerun_no_tags.jpg", texture1, width, height);
	loadTexture("../res/textures/faerun_tags.jpg", texture2, width, height);

	//calculate lower y-coordinates based on texture size
	float yVal = ((float)height / (float)width) - 1.0f;
	std::cout << yVal << std::endl;

	float vertices[] = 
	{
		// positions			// texture coords
		 1.0f,  1.0f, 0.0f,		1.0f, 1.0f, // top right
		 1.0f,  yVal, 0.0f,		1.0f, 0.0f, // bottom right
		-1.0f,  yVal, 0.0f,		0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,		0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = 
	{
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ourShader.use(); 

	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1);

	bool show_demo_window = false;
	bool labels = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(window)) 
	{
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Control Panel"); 
		ImGui::Checkbox("Labels", &labels);

		if (ImGui::Button("Reset")) 
		{
			viewVec = glm::vec3(0.0f, 0.0f, -3.0f);
		}

		//ImGui::SliderFloat("float", &mixValue, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		
		glm::mat4 view(1);
		glm::mat4 projection(1);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.00000001f, 100.0f);
		view = glm::translate(view, viewVec);

		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		mixValue = labels ? 1.0f : 0.0f;
		ourShader.setFloat("mixValue", mixValue);

		glBindVertexArray(VAO);

		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void loadTexture(const char* filename, unsigned int& texture1, int& width, int& height) 
{
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded textures on the y-axis.
	std::cout << "Loading image data..." << std::endl;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data) {
		std::cout << "Done loading image data. Generating texture..." << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		std::cout << "Done generating texture. Generating mipmaps..." << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Done generating mipmaps." << std::endl;
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void processInput(GLFWwindow *window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//pan slower when zoomed in further
	float panSpeed = abs(viewVec.z);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		viewVec.y -= 0.00625 * panSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		viewVec.y += 0.00625 * panSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		viewVec.x += 0.00625 * panSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		viewVec.x -= 0.00625 * panSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		if(viewVec.z >= -20.0f)
			viewVec.z -= 0.00125;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		if(viewVec.z <= -0.01)
			viewVec.z += 0.00125;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}

void scroll_callback(GLFWwindow*, double xoffset, double yoffset) 
{
	double yZoom = yoffset;

	if (yZoom >= 0.5f)
		yZoom = 0.5f;
	if (yZoom <= -0.5f)
		yZoom = -0.5f;

	float zoomAmt = abs(viewVec.z) / 5;

	viewVec.z += yZoom * zoomAmt;

	if (viewVec.z <= -20.0f)
		viewVec.z = -20.0f;
	if (viewVec.z >= -0.01f)
		viewVec.z = -0.01f;
}