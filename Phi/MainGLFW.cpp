#include "PreCompile.h"
#include <Game/Phi.h>

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}




int main(int argc, char* argv[])
{
    GLFWwindow* window;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glfwSetKeyCallback(window, key_callback);

    Game::Phi game;
    Game::Initialize args;
    game.initialize(args);

    Game::Update update;
    update.window = window;
    
    
    float previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        {
            double time = glfwGetTime();
            update.delta = (float)(previousTime - time);
            game.update(update);
            previousTime = time;
        }
        {
            Game::RenderArg render;
            render.projection = update.projection;
            render.view = update.view;
            render.from = update.from;
            render.to = update.to;
            game.render(render);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);

}