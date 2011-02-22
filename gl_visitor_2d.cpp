#include "gl_visitor.hpp"

#include <GL/glfw.h>
#include <cmath>

namespace trivial
{
namespace impl
{
    gl_visitor_2d::gl_visitor_2d() : point_size_(1.0f), x_(0), y_(0)
    {
        glPointSize(point_size_);
    }

    void gl_visitor_2d::update_view ()
    {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(x_ - width / (2 * point_size_),
                x_ + width / (2 * point_size_),
                y_ - height / (2 * point_size_),
                y_ + height / (2 * point_size_),
                -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPointSize(point_size_);
    }

    bool gl_visitor_2d::start_frame()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        return false;
    }

    void gl_visitor_2d::draw_sphere (vector<2> const& v, float radius)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_STRIP);

        for(unsigned n = 0; n <= tesselation_; ++n)
            glVertex2f(
                  v[0] + radius * sin(n * (2 * M_PI / tesselation_)),
                  v[1] + radius * cos(n * (2 * M_PI / tesselation_))
                ); 
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
    }

    void gl_visitor_2d::draw_box (vector<2> const& v)
    {
        glBegin(GL_POINTS);
        glVertex2i(v[0], v[1]);
        glEnd();
    }

    void gl_visitor_2d::click_event (int x, int y)
    {
        x_ += (x - ((int)width / 2)) / point_size_;
        y_ -= (y - ((int)height / 2)) / point_size_;
        view_changed = true;
    }

    void gl_visitor_2d::wheel_event(int wheel)
    {
        point_size_ *= std::pow(2.0f, wheel);
        view_changed = true;
    }

}
}
