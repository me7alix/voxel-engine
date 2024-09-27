#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "octree/octree.h"
#include "linmath.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

unsigned vao;
unsigned int shader_program;

const char* vertex_shader_source =
        "#version 430 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"out vec3 vertexColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(position, 1.0);\n"
		"    vertexColor = color;\n"
		"}\n";


char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s for reading\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* content = (char*)malloc((length + 1) * sizeof(char));
    if (!content) {
        fprintf(stderr, "Could not allocate memory for file contents\n");
        fclose(file);
        return NULL;
    }
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);
    return content;
}

void setup_shader() 
{
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    char* fragment_shader_source = read_file("../src/shaders/shader.frag");
    if (!fragment_shader_source) {
        fprintf(stderr, "Failed to load fragment shader\n");
        return;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char**)&fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // Check for compilation errors in the fragment shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    // Clean up shaders as they are no longer needed after linking
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Free the memory allocated for the fragment shader source
    free(fragment_shader_source);
}

void setup_data() 
{
	float vertices[] = {
	  -1.0f, -1.0f, 0.0f, 1.0, 1.0, 1.0,
	   1.0f, -1.0f, 0.0f, 1.0, 1.0, 1.0,
	  -1.0f,  1.0f, 0.0f, 1.0, 1.0, 1.0,
       1.0f,  1.0f, 0.0f, 1.0, 1.0, 1.0,
	   1.0f, -1.0f, 0.0f, 1.0, 1.0, 1.0,
	  -1.0f,  1.0f, 0.0f, 1.0, 1.0, 1.0,
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

float p_x = 0, p_y = 0, p_z = -3, a_x = 0, a_y = 0;
int dpx = 0, dpy = 0, dpz = 0, dax = 0, day = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        dpz = 1;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        dpz = -1;
    if ((key == GLFW_KEY_S || key == GLFW_KEY_W) && action == GLFW_RELEASE)
        dpz = 0;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        dpx = -1;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        dpx = 1;
    if ((key == GLFW_KEY_A || key == GLFW_KEY_D) && action == GLFW_RELEASE)
        dpx = 0;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        dpy = 1;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        dpy = -1;
    if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_SPACE) && action == GLFW_RELEASE)
        dpy = 0;
}

double lastCursorX = SCREEN_WIDTH / 2.0, lastCursorY = SCREEN_HEIGHT / 2.0;
double deltaCursorX = 0.0, deltaCursorY = 0.0;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    deltaCursorX = xpos - lastCursorX;
    deltaCursorY = ypos - lastCursorY;

    lastCursorX = xpos;
    lastCursorY = ypos;
}

void spherical_to_cartesian(float r, float theta, float phi, vec3 result) {
    result[0] = r * sinf(theta) * cosf(phi); // x
    result[1] = r * sinf(theta) * sinf(phi); // y
    result[2] = r * cosf(theta);             // z
}

OctreeArray *octarr;
GLuint ssbo;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    vec3 pos = {0, 0, 0};
    vec3 sp = {0, 0, 0};
    //spherical_to_cartesian(1.0, -p_x + PI/2.0, PI / 2.0, sp);
    sp[0] += p_x; sp[1] += p_y; sp[2] += p_z;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        destroyVoxels(octarr, 0, pos, sp, 0.7, 0);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        addVoxels(octarr, 0, pos, sp, 0.4, 0);
    }
    glDeleteBuffers(1, &ssbo);
    GLuint ssbo = setupSSBO(octarr->arr, shader_program);
}

int main(void)
{
    GLFWwindow* window;
    if (!glfwInit()){
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", glfwGetPrimaryMonitor(), 0);

    if (!window){
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    	glfwTerminate();
        return -1;
    }

    setup_shader();
    setup_data();

    octarr = octarr_new();
    Octree root;
    root.isColored = 1;
    root.isIntact = 1;
    vec3 pos = {0, 0, 0};
    vec3 sp = {0, 0, 0};
    octarr_add(octarr, root);
    destroyVoxels(octarr, 0, pos, sp, 2.7, 0);
    float movingSpeed = 0.03;
    ssbo = setupSSBO(octarr->arr, shader_program);
    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        a_x += (float)(deltaCursorX / SCREEN_WIDTH * PI * 2.0 * 1.5);
        a_y += (float)(deltaCursorY / SCREEN_WIDTH * PI * 2.0 * 1.5);
        p_z += movingSpeed * dpz * sinf(a_x + PI / 2.0);
        p_x += movingSpeed * dpz * cosf(a_x + PI / 2.0);
        p_z += movingSpeed * dpx * sinf(a_x + PI);
        p_x += movingSpeed * dpx * cosf(a_x + PI);
        p_y += movingSpeed * dpy;


        int loc = glGetUniformLocation(shader_program, "res");
        glUniform2f(loc, (float) SCREEN_WIDTH, (float) SCREEN_HEIGHT);

        loc = glGetUniformLocation(shader_program, "position");
        glUniform3f(loc, p_x, p_y, p_z);

        loc = glGetUniformLocation(shader_program, "angles");
        glUniform2f(loc, a_x, a_y);

		//GLuint ssbo = setupSSBO(octarr->arr, shader_program);

        glfwSetCursorPos(window, SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
        lastCursorX = SCREEN_WIDTH / 2.0;
        lastCursorY = SCREEN_HEIGHT / 2.0;

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDeleteBuffers(1, &ssbo);

        deltaCursorX = 0; deltaCursorY = 0;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}