#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int setupGeometry();

const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX=false, rotateY=false, rotateZ=false;
float moveX = -0.5f, moveY = 0.0f, moveZ = 0.0f;
float scale = 0.5f;

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M2: Instanciando objetos na cena 3D - Thalia Schwaab", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader = Shader("../shaders/hello.vs", "../shaders/hello.fs");

	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1); 

		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		model = glm::translate(model, glm::vec3(moveX, moveY, moveZ));
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawArrays(GL_POINTS, 0, 36);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.5, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawArrays(GL_POINTS, 0, 36);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
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

	//translação

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		moveY += 0.01f;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		moveY -= 0.01f;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		moveX -= 0.01f;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		moveX += 0.01f;
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		moveZ -= 0.01f;
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		moveZ += 0.01f;
	}

	//escala

	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
	{
		scale -= 0.01f;
	}

	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
	{
		scale += 0.01f;
	}
}

int setupGeometry()
{
	GLfloat vertices[] = {
		// Face 1 - Vermelho
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

		// Face 2 - Verde
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,

		// Face 3 - Azul
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

		// Face 4 - Amarelo
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

		 // Face 5 - Azul claro
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		 // Face 6 - Rosa
		 -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

