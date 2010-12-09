#include "window.hpp"

#include <GL/glfw.h>

namespace simple
{

    window::window (unsigned width, unsigned height, bool fullscreen)
    {
        glfwInit();

        if ( !glfwOpenWindow (width, height, 0, 0, 0, 0, 16, 0,
                              fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) )
            throw "window: Kaputt";

        glfwSwapInterval (0);

        glDisable(GL_DEPTH_TEST);

        glShadeModel(GL_FLAT);
        glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glClearColor (0, 0, 0, 0);
        glColor3f (1, 1, 1);

        glOrtho ( 0, width, height, 0, -1, 1);
    }

    window::~window()
    {
        glfwTerminate ();
    }
}

