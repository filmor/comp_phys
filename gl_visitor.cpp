#include "gl_visitor.hpp"

#include <GL/glfw.h>

namespace trivial
{
namespace impl
{
    gl_visitor_base::gl_visitor_base() : tesselation_(24),
                                         last_mouse_event_(0)
    {
        glfwInit();
        glfwOpenWindow(400, 400, 8, 8, 8, 8, 8, 0, GLFW_WINDOW);
        glfwSetWindowSizeCallback(resize);
        glfwEnable(GLFW_STICKY_MOUSE_BUTTONS);
    }

    bool gl_visitor_base::view_changed = false;
    unsigned gl_visitor_base::width = 400;
    unsigned gl_visitor_base::height = 400;

    gl_visitor_base::~gl_visitor_base() { glfwTerminate(); }

    void GLFWCALL gl_visitor_base::resize (int width_, int height_)
    {
        width = width_;
        height = height_;
        view_changed = true;
    }

    void gl_visitor_base::handle_input()
    {
        if(int mouse_wheel = glfwGetMouseWheel())
        {
            glfwSetMouseWheel(0);
            wheel_event(mouse_wheel);
        }

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) &&
           glfwGetTime() - last_mouse_event_ > 0.5)
        {
            int x, y;
            glfwGetMousePos(&x, &y);
            last_mouse_event_ = glfwGetTime();
            click_event(x, y);
        }
    }

    void gl_visitor_base::end_frame()
    {
        glfwSwapBuffers();
    }

}
}
