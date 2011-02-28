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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    }

    void gl_visitor_2d::draw_box (vector<2> const& v, float r, float g, float b)
    {
        glColor3f(r, g, b);
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

    void gl_visitor_2d::draw_particle(const eden::static_particle<position<2>> & p, const vector<2> & offset)
    {
        float r, g, b;
        set_gradient_color(p.score * 1.0, r, g, b);
        draw_box(p.position + offset, r, g, b);
    }

    void gl_visitor_2d::set_gradient_color(float value, float & r, float & g, float & b)
    {
        const float a1 = 0.16f, a2 = 0.33f, a3 = 0.50f, a4 = 0.66f, a5 = 0.83f, a6 = 1.00f;

        if(value < a2)
        { r = 0; g = std::max(0.0f, (value - a1)/(a2 - a1)); b = 1; }
        else if(value < a3)
        { r = 0; g = 1; b = std::cos((a2 - value)/(a3 - a2) * M_PI * 0.5f); }
        else if(value < a4)
        { r = std::sin((value - a3)/(a4 - a3) * M_PI * 0.5f); g = 1; b = 0; }
        else if(value < a5)
        { r = 1; g = std::max(0.0f, (a5 - value)/(a5 - a4)); b = 0; }
        else
        { r = 1; g = 0; b = std::min(0.8f, (value - a5)/(a6 - a5)); }
    }
}
}
