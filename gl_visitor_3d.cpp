#include "gl_visitor.hpp"

#include <GL/glfw.h>

namespace trivial
{
namespace impl
{

    gl_visitor_3d::gl_visitor_3d() : anaglyph_(0),
                                     distance_(100.0f), angle_(0),
                                     left_(true),
                                     quadric_(gluNewQuadric())
    {
        glEnable(GL_CULL_FACE);

        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 1.0f);
        glFogf(GL_FOG_END, 2 * distance_);
        glEnable(GL_FOG);
        glPolygonMode(GL_BACK, GL_LINE);

        float pos[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);

        glEnable(GL_DEPTH_TEST);

        cube_list_ = glGenLists(1);
        glNewList(cube_list_, GL_COMPILE);
            glBegin(GL_QUADS);
                //back
                glNormal3f(0.0f, 0.0f, -1.0f);
                glVertex3f(-0.5f, -0.5f, -0.5f);
                glVertex3f(-0.5f, 0.5f, -0.5f);
                glVertex3f(0.5f, 0.5f, -0.5f);
                glVertex3f(0.5f, -0.5f, -0.5f); 
                //top
                glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(-0.5f, 0.5f, -0.5f);
                glVertex3f(-0.5f, 0.5f, 0.5f);
                glVertex3f(0.5f, 0.5f, 0.5f);
                glVertex3f(0.5f, 0.5f, -0.5f); 
                //front
                glNormal3f(0.0f, 0.0f, 1.0f);
                glVertex3f(-0.5f, -0.5f, 0.5f);
                glVertex3f(0.5f, -0.5f, 0.5f);
                glVertex3f(0.5f, 0.5f, 0.5f);
                glVertex3f(-0.5f, 0.5f, 0.5f); 
                //bottom
                glNormal3f(0.0f, -1.0f, 0.0f);
                glVertex3f(-0.5f, -0.5f, -0.5f);
                glVertex3f(0.5f, -0.5f, -0.5f);
                glVertex3f(0.5f, -0.5f, 0.5f);
                glVertex3f(-0.5f, -0.5f, 0.5f);
                //left
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(-0.5f, -0.5f, -0.5f);
                glVertex3f(-0.5f, -0.5f, 0.5f);
                glVertex3f(-0.5f, 0.5f, 0.5f);
                glVertex3f(-0.5f, 0.5f, -0.5f);
                //right
                glNormal3f(1.0f, 0.0f, 0.0f);
                glVertex3f(0.5f, -0.5f, -0.5f);
                glVertex3f(0.5f, 0.5f, -0.5f);
                glVertex3f(0.5f, 0.5f, 0.5f);
                glVertex3f(0.5f, -0.5f, 0.5f);
            glEnd();
        glEndList();
    }

    void gl_visitor_3d::update_view()
    {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0f, (float)width / height,
                       1.0f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0, 0, -distance_);
        glRotatef(angle_, 0.0f, 1.0f, 0.0f);
    }

    bool gl_visitor_3d::start_frame()
    {
        if (anaglyph_)
        {
            int ana = anaglyph_;

            if (left_ == false)
                ana >>= 4;
            glColor3f(1.0f, 1.0f, 1.0f);

            glColorMask(ana & ANA_RED,
                        ana & ANA_GREEN,
                        ana & ANA_BLUE,
                        false);

            // focal point is origin; eye separation e considered "stressless" if
            // e = distance_ / 60, i.e. to first order at an angle 3/pi
            if (left_ == false)
                glRotatef(-3.0f / M_PI, 0.0f, 1.0f, 0.0f);

            left_ = !left_;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        return anaglyph_ && !left_;
    }

    void gl_visitor_3d::end_frame()
    {
        if (anaglyph_)
            glRotatef(3.0f / M_PI, 0.0f, 1.0f, 0.0f);
        gl_visitor_base::end_frame();
    }

    gl_visitor_3d::~gl_visitor_3d()
    {
        glDeleteLists(cube_list_, 1);
    }

    void gl_visitor_3d::click_event(int x, int y)
    {
        angle_ += (x - (int)width / 2) * 0.1f;
        view_changed = true;
    }

    void gl_visitor_3d::wheel_event(int wheel)
    {
        distance_ -= 10 * wheel;
        glFogf(GL_FOG_END, 2 * distance_);
        view_changed = true;
    }

    void gl_visitor_3d::draw_sphere(vector<3> const& v, float radius)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT, GL_LINE);
        glPushMatrix();
        glTranslatef(v[0], v[1], v[2]);
        gluSphere((GLUquadric*)quadric_, radius, tesselation_, tesselation_);
        glPopMatrix();
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
    }

    void gl_visitor_3d::draw_box(vector<3> const& v)
    {
        glPushMatrix();
        glTranslatef(v[0], v[1], v[2]);
        glCallList(cube_list_);
        glPopMatrix();
    }
    
}
}
