#ifndef TRIVIAL_GL_VISITOR_HPP
#define TRIVIAL_GL_VISITOR_HPP

#include <GL/glfw.h>
#include <stdexcept>

#include "visitor.hpp"

#define CLUSTER_CIRCLE_SEGMENTS 36
#define ANA_LR 1
#define ANA_LG 2
#define ANA_LB 4
#define ANA_RR 8
#define ANA_RG 16
#define ANA_RB 32

namespace trivial
{

    template<class World>
    class gl_visitor_2d;
    template<class World>
    class gl_visitor_3d;

    template<class World>
    class gl_visitor : public const_world_visitor<World>
    {
    public:
        static gl_visitor & visitor()
        {
            if(World::position_type::dimension == 2)
            {
                static gl_visitor_2d<World> v;
                visitor_ = &v;
                return v;
            }
            else if(World::position_type::dimension == 3)
            {
                static gl_visitor_3d<World> v;
                visitor_ = &v;
                return v;
            }
            else
                throw std::logic_error(
                        "gl_visitor: dimension other than 2 or 3 not supported."
                        );
        }

        bool limit_fps;
        unsigned tesselation;

        void visit (World const& world)
        {
            glFlush();
            glfwSwapBuffers();

            if(limit_fps)
                glfwSleep(0.1);
            
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

    protected:
        int width_, height_;
        double last_mouse_event_;
        bool view_changed_;

        gl_visitor() : limit_fps(false), tesselation(24),
                       width_(400), height_(400),
                       last_mouse_event_(0), view_changed_(true)
        {
            glfwInit();
            glfwOpenWindow(width_, height_, 8, 8, 8, 8, 8, 0, GLFW_WINDOW);
            glfwSetWindowSizeCallback(resize);
            glfwEnable(GLFW_STICKY_MOUSE_BUTTONS);
        }

        ~gl_visitor()
        {
            glfwTerminate();
        }

    private:
        static gl_visitor* visitor_;

        static void GLFWCALL resize(int width, int height)
        {
            if(!visitor_)
                return;

            visitor_->width_ = width;
            visitor_->height_ = height;
            visitor_->view_changed_ = true;
        }

        virtual void click_event(int x, int y) = 0;
        virtual void wheel_event(int wheel) = 0;
    };

    template<class World>
    gl_visitor<World>* gl_visitor<World>::visitor_ = 0;

    template<class World>
    class gl_visitor_2d : gl_visitor<World>
    {
        friend class gl_visitor<World>;

    public:
        void visit(typename World::particle_type const& particle)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex2i(particle.position[0], particle.position[1]);
            glEnd();
        }

        void visit(typename World::cluster_type const& cluster)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINE_STRIP);

            for(unsigned n = 0; n <= this->tesselation; ++n)
                glVertex2f(
                      cluster.get_center()[0]
                      + cluster.get_radius()
                        * sin(n * (2 * M_PI / this->tesselation)),
                      cluster.get_center()[1]
                      + cluster.get_radius()
                        * cos(n * (2 * M_PI / this->tesselation))
                    ); 
            glEnd();

            // TODO: MultiIndex: We need a vector of positions of particles
            //       as well as a map of positions to particles
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POINTS);
            for(unsigned n = 0; n < cluster.get_particles().size(); ++n)
            {
                glVertex2i(
                        cluster.get_particles()[n].position[0],
                        cluster.get_particles()[n].position[1]
                        );
            }
            glEnd();
        }

        void visit(World const& world)
        {
            gl_visitor<World>::visit(world);

            if(this->view_changed_)
            {
                glViewport(0, 0, this->width_, this->height_);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(x_ - this->width_ / (2 * this->point_size_),
                        x_ + this->width_ / (2 * this->point_size_),
                        y_ - this->height_ / (2 * this->point_size_),
                        y_ + this->height_ / (2 * this->point_size_),
                        -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glPointSize(this->point_size_);
                this->view_changed_ = false;
            }
            glClear(GL_COLOR_BUFFER_BIT);
        }

    private:
        float point_size_;
        int x_, y_;

        gl_visitor_2d() : point_size_(1.0f), x_(0), y_(0)
        {
            glPointSize(point_size_);
        }

        void click_event(int x, int y)
        {
            x_ += (x - (this->width_ / 2)) / this->point_size_;
            y_ -= (y - (this->height_ / 2)) / this->point_size_;
            this->view_changed_ = true;
        }

        void wheel_event(int wheel)
        {
            this->point_size_ *= std::pow(2.0f, wheel);
            this->view_changed_ = true;
        }
    };

    template<class World>
    class gl_visitor_3d : gl_visitor<World>
    {
        friend class gl_visitor<World>;

    public:
        char anaglyph;

        void visit(typename World::particle_type const& particle)
        {
            if(anaglyph)
                // have to keep special drawing order and do proper
                // colouring for anaglyphs
                return;
            glColor3f(1.0f, 1.0f, 1.0f);
            render(particle);
        }

        void visit (typename World::cluster_type const& cluster)
        {
            if(anaglyph)
                // have to keep special drawing order and do proper
                // colouring for anaglyphs
                return;
            glColor3f(1.0f, 0.0f, 0.0f);
            render(cluster);
        }

        void visit(World const& world)
        {
            gl_visitor<World>::visit(world);

            if(this->view_changed_)
            {
                glViewport(0, 0, this->width_, this->height_);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(90.0f, (float)this->width_ / this->height_,
                               1.0f, 1000.0f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(0, 0, -distance_);
                glRotatef(angle_, 0.0f, 1.0f, 0.0f);
                this->view_changed_ = false;
            }
    
            if(!anaglyph)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            else
            {
                glColor3f(1.0f, 1.0f, 1.0f);

                glColorMask(anaglyph & ANA_LR, anaglyph & ANA_LG, anaglyph & ANA_LB, false);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                render(world);
                
                glColorMask(anaglyph & ANA_RR, anaglyph & ANA_RG, anaglyph & ANA_RB, false);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // focal point is origin; eye separation e considered "stressless" if
                // e = distance_ / 60, i.e. to first order at an angle 3/pi
                glRotatef(-3.0f / M_PI, 0.0f, 1.0f, 0.0f);
                render(world);
                glRotatef(3.0f / M_PI, 0.0f, 1.0f, 0.0f);
            }
        }

    private:
        float distance_, angle_;
        int cube_list_;
        GLUquadricObj* quadric_;

        gl_visitor_3d() : anaglyph(0), distance_(100.0f), angle_(0),
                          quadric_(gluNewQuadric())
        {
            glEnable(GL_CULL_FACE);

            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, 1.0f);
            glFogf(GL_FOG_END, 2 * distance_);
            glEnable(GL_FOG);

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

        ~gl_visitor_3d()
        {
            glDeleteLists(cube_list_, 1);
        }

        void click_event(int x, int y)
        {
            angle_ += (x - (this->width_ / 2)) * 0.1f;
            this->view_changed_ = true;
        }

        void wheel_event(int wheel)
        {
            distance_ -= 10 * wheel;
            glFogf(GL_FOG_END, 2 * distance_);
            this->view_changed_ = true;
        }

        void render(typename World::particle_type const& particle)
        {
            glPushMatrix();
            glTranslatef(particle.position[0],
                         particle.position[1],
                         particle.position[2]);
            glCallList(cube_list_);
            glPopMatrix();
        }

        void render(typename World::cluster_type const& cluster)
        {
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT, GL_LINE);
            glPushMatrix();
            glTranslatef(cluster.get_center()[0],
                         cluster.get_center()[1],
                         cluster.get_center()[2]);
            gluSphere(quadric_, cluster.get_radius(), this->tesselation,
                      this->tesselation);
            glPopMatrix();
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_LIGHTING);

            for(unsigned n = 0; n < cluster.get_particles().size(); ++n)
                render(cluster.get_particles()[n]);
        }

        void render(World const& world)
        {
            for(unsigned n = 0; n < world.get_particles().size(); ++n)
                render(world.get_particles()[n]);

            for(unsigned n = 0; n < world.get_clusters().size(); ++n)
                render(world.get_clusters()[n]);
        }
    };
}

#endif
