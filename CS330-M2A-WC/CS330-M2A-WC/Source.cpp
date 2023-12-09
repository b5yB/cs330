#include <iostream>         
#include <cstdlib>          
#include <GL/glew.h>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

namespace {

	const char* const windowTitle = "CS330-M2A-WC";
	const int windowWidth = 800;
	const int windowHeight = 600;

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

const char *vertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 colorFromVBO;\n"
"out vec4 colorFromVS;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   colorFromVS = colorFromVBO;\n"
"}\n\0";

const char *fragmentShaderSource = "#version 440 core\n"
"in vec4 colorFromVS;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = colorFromVS;\n"
"}\n\0";


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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(gProgramId);
	glBindVertexArray(gMesh.vao);
	glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
	glfwSwapBuffers(gWindow);
}

void UCreateMesh(GLMesh &mesh) {

	GLfloat verts[] = {

		// index 0
		-1.0f, 1.0f, 0.0f,      // top left
		1.0f, 0.0f, 0.0f, 1.0f, // red

		// index 1
		-1.0f, 0.0f, 0.0f,     // middle left
		0.0f, 0.0f, 1.0f, 1.0f, // blue

		// index 2
		-0.5f, -0.0f, 0.0f,      // middle
		0.0f, 1.0f, 0.0f, 1.0f, // green

		// index 3
		0.0f, 0.0f, 0.0f,       // middle right
		1.0f, 0.0f, 0.0f, 1.0f, // red

		// index 4
		0.0f, -1.0f, 0.0f,      // bottom right
		0.0f, 1.0f, 0.0f, 1.0f  // green
	};

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	GLushort indices[] = { 0, 1, 2, 3, 2, 4 };
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