/* Modulo 6 - Adaptado por Thalia Schwaab */
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Bezier.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int loadTexture(string path);
void loadOBJ(string path);
void loadMTL(string path);
int setupGeometry();

std::vector<glm::vec3> generateUnisinosPointsSet(string path);

vector<GLfloat> totalvertices;
vector<GLfloat> vertices;
vector<GLfloat> textures;
vector<GLfloat> normais;

string mtlFilePath = "SuzanneTriTextured.mtl";
string textureFilePath = "Suzanne.png";
string objFileName = "SuzanneTriTextured.obj";

vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;

const GLuint WIDTH = 800, HEIGHT = 600;

bool rotateX = false, rotateY = false, rotateZ = false;
float scale = 0.5f;

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05f;
float pitch = 0.0, yaw = -90.0;

Camera camera;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M6: Definindo trajetórias para alguns objetos - Thalia Schwaab", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	Shader shader("../shaders/hello.vs", "../shaders/hello.fs");
	loadOBJ("../objects/" + objFileName);
	loadMTL("../textures/" + mtlFilePath);

	GLuint textureID = loadTexture(textureFilePath);
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	Mesh object;
	object.initialize(VAO, (totalvertices.size() / 8), &shader);

	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.7);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	camera.initialize(&shader, width, height);

	std::vector<glm::vec3> basePoints = generateUnisinosPointsSet("../points.txt");

	Bezier bezier;
	bezier.setControlPoints(basePoints);
	bezier.setShader(&shader);
	bezier.generateCurve(30);

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;
	int speed = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);

		if (speed++ == 50) {
			i = (i + 1) % nbCurvePoints;
			speed = 0;
		}

		camera.update();

		object.update(pointOnCurve, rotateX, rotateY, rotateZ, scale);
		object.draw(textureID);

		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void loadMTL(string path)
{
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." + path << std::endl;
	}

	std::string line;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "map_Kd")
			{
				iss >> textureFilePath;
			}
			else if (prefix == "Ka")
			{
				glm::vec3 temp_ka;
				iss >> temp_ka.x >> temp_ka.y >> temp_ka.z;

				ka.push_back(temp_ka.x);
				ka.push_back(temp_ka.y);
				ka.push_back(temp_ka.z);
			}
			else if (prefix == "Ks")
			{
				glm::vec3 temp_ks;
				iss >> temp_ks.x >> temp_ks.y >> temp_ks.z;

				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
			}
			else if (prefix == "Ns")
			{
				iss >> ns;
			}
		}
	}

	file.close();
}

int setupGeometry()
{
	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, totalvertices.size() * sizeof(GLfloat), totalvertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

void loadOBJ(string path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." + path << std::endl;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_textures;
	std::vector<glm::vec3> temp_normais;

	std::string line;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);

			std::string prefix;
			iss >> prefix;

			if (prefix == "v") {

				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_vertices.push_back(values);
			}
			else if (prefix == "vt")
			{
				glm::vec2 values;
				iss >> values.x >> values.y;
				temp_textures.push_back(values);
			}
			else if (prefix == "vn")
			{
				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_normais.push_back(values);
			}
			else if (prefix == "f")
			{
				unsigned int vertexIndex, textIndex, normalIndex;
				char slash;

				for (int i = 0; i < 3; ++i)
				{
					iss >> vertexIndex >> slash >> textIndex >> slash >> normalIndex;

					glm::vec3 verticess = temp_vertices[vertexIndex - 1];
					glm::vec3 normaiss = temp_normais[normalIndex - 1];
					glm::vec2 texturess = temp_textures[textIndex - 1];

					totalvertices.push_back(verticess.x);
					totalvertices.push_back(verticess.y);
					totalvertices.push_back(verticess.z);

					totalvertices.push_back(texturess.x);
					totalvertices.push_back(texturess.y);

					totalvertices.push_back(normaiss.x);
					totalvertices.push_back(normaiss.y);
					totalvertices.push_back(normaiss.z);
				}
			}
			else if (prefix == "mtllib")
			{
				iss >> mtlFilePath;
			}
		}
	}

	file.close();
}

int loadTexture(string path)
{
	GLuint texID;

	cout << "trying to load texture" + path << endl;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" + path << endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.mouseCallback(window, xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;

	}

	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
	{
		scale -= 0.01f;
	}

	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
	{
		scale += 0.01f;
	}
	camera.setCameraPos(key);
}

std::vector<glm::vec3> generateUnisinosPointsSet(string path)
{
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." + path << std::endl;
	}

	std::string line;
	vector <glm::vec3> points;

	while (std::getline(file, line))
		if (line.length() > 0) {

			std::istringstream iss(line);
			std::string prefix;
			char comma;

			glm::vec3 temp_points;
			iss >> temp_points.x >> comma >> temp_points.y >> comma >> temp_points.z;

			points.push_back(temp_points);
		}

	file.close();

	return points;
} 