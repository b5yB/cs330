#include <iostream>         
#include <cstdlib>          
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

namespace {

	const char* const windowTitle = "CS330-M3A-WC";
	const int windowWidth = 800;
	const int windowHeight = 700;

	struct GLMesh {
		GLuint vao;
		GLuint vbos[2];
		GLuint nIndices;
	};

	GLFWwindow* gWindow = nullptr;
	GLMesh gMesh;
	GLuint gProgramId;
}

bool UInitialize(int, char*[], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh &mesh);
void UDestroyMesh(GLMesh &mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId);
void UDestroyShaderProgram(GLuint programId);

const GLchar * vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position;
	layout(location = 1) in vec4 color;
	out vec4 vertexColor;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
		vertexColor = color; // references incoming color data
	}
);

const GLchar * fragmentShaderSource = GLSL(440,
	in vec4 vertexColor;
	out vec4 fragmentColor;

	void main() {
	fragmentColor = vec4(vertexColor);
    }
);

int main(int argc, char* argv[]) {

	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	UCreateMesh(gMesh);

	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(gWindow)) {
		UProcessInput(gWindow);
		URender();
		glfwPollEvents();
	}

	UDestroyMesh(gMesh);
	UDestroyShaderProgram(gProgramId);
	exit(EXIT_SUCCESS);
}

bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	*window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

	if (*window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);


	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
	return true;
}

void UProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void URender() {

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
	glm::mat4 rotation = glm::rotate(0.5f, glm::vec3(0.0, 1.0f, 0.0f));
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 model = translation * rotation * scale;
	glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	glUseProgram(gProgramId);

	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(gProgramId);
	glBindVertexArray(gMesh.vao);
	glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
	glfwSwapBuffers(gWindow);
}

void UCreateMesh(GLMesh &mesh) {

	GLfloat vertices[] = {

		// 0
		-0.5f, -0.5f, 0.5f,     // front left
		1.0f, 0.0f, 0.0f, 0.0f, // red

		// 1
		-0.5f, -0.5f, -0.5f,    // back left
		0.0f, 1.0f, 0.0f, 0.0f, // green

		// 2
		0.5f, -0.5f, -0.5f,     // back right
		0.0f, 0.0f, 1.0f, 0.0f, // blue

		// 3
		0.5f, -0.5f, 0.5f,      // front right
		1.0f, 1.0f, 0.0f, 0.0f, // yellow

		// 4
		0.0f, 0.5f, 0.0f,      // peak
		0.0f, 1.0f, 1.0f, 0.0f  // cyan
	};

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLushort indices[] = { 
		0, 1, 2, // base left
		0, 2, 3, // base right
		0, 1, 4, // left
		1, 2, 4, // back
		2, 3, 4, // right
		3, 0, 4  // front
	};

	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerColor = 4;

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}

void UDestroyMesh(GLMesh &mesh) {
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}

bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId) {

	int success = 0;
	char infoLog[512];
	programId = glCreateProgram();
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);
	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glCompileShader(fragmentShaderId);

	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glUseProgram(programId);
	return true;
}

void UDestroyShaderProgram(GLuint programId) {
	glDeleteProgram(programId);
}