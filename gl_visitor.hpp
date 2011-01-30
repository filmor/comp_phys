#ifndef TRIVIAL_GL_VISITOR_HPP
#define TRIVIAL_GL_VISITOR_HPP

#include <GL/glfw.h>

#include "visitor.hpp"

namespace trivial
{
	template<typename Position, class Particle, class Cluster, class World>
	class gl_visitor_2d;
	template<typename Position, class Particle, class Cluster, class World>
	class gl_visitor_3d;

	template<typename Position, class Particle, class Cluster, class World>
	class gl_visitor : public omni_visitor<Position, World>
	{
		public:
			static gl_visitor & visitor()
			{
				if(Position::dimension == 2)
				{
					static gl_visitor_2d<Position, Particle, Cluster, World> v;
					visitor_ = &v;
					return v;
				}
				else if(Position::dimension == 3)
				{
					static gl_visitor_3d<Position, Particle, Cluster, World> v;
					visitor_ = &v;
					return v;
				}
				else throw "gl_visitor: dimension other than 2 or 3 not supported.";
			}

			bool limit_fps;
			unsigned tesselation;

			void visit(World * world)
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
				if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) && glfwGetTime() - last_mouse_event_ > 0.5)
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

			gl_visitor() : limit_fps(false), tesselation(24), width_(400), height_(400), last_mouse_event_(0), view_changed_(true)
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
			static gl_visitor * visitor_;

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

	template<typename Position, class Particle, class Cluster, class World>
	gl_visitor<Position, Particle, Cluster, World> * gl_visitor<Position, Particle, Cluster, World>::visitor_ = 0;

	template<typename Position, class Particle, class Cluster, class World>
	class gl_visitor_2d : gl_visitor<Position, Particle, Cluster, World>
	{
		friend class gl_visitor<Position, Particle, Cluster, World>;

		public:
			void visit(particle<Position> * particle)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_POINTS);
				glVertex2f(particle->position.to_array()[0], particle->position.to_array()[1]);
				glEnd();
			}

			void visit(cluster<Position> * cluster)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_LINE_STRIP);
				for(unsigned n = 0; n <= this->tesselation; ++n)
					glVertex2f(cluster->get_center().to_array()[0] + cluster->get_radius() * sin(n * (2 * M_PI / this->tesselation)), cluster->get_center().to_array()[1] + cluster->get_radius() * cos(n * (2 * M_PI / this->tesselation))); 
				glEnd();

				glColor3f(1.0f, 0.0f, 0.0f);
				glBegin(GL_POINTS);
				for(unsigned n = 0; n < cluster->get_particles().size(); ++n)
					glVertex2f(cluster->get_particles()[n]->position.to_array()[0], cluster->get_particles()[n]->position.to_array()[1]);
				glEnd();
			}

			void visit(World * world)
			{
				gl_visitor<Position, Particle, Cluster, World>::visit(world);

				if(this->view_changed_)
				{
					glViewport(0, 0, this->width_, this->height_);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho(x_ - this->width_ / (2 * point_size_), x_ +  this->width_ / (2 * point_size_), y_ - this->height_ / (2 * point_size_), y_ + this->height_ / (2 * point_size_), -1, 1);
					glMatrixMode(GL_MODELVIEW);
					glPointSize(point_size_);
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
				x_ += (x - (this->width_ / 2)) / point_size_;
				y_ -= (y - (this->height_ / 2)) / point_size_;
				this->view_changed_ = true;
			}

			void wheel_event(int wheel)
			{
				point_size_ *= std::pow(2.0f, wheel);
				this->view_changed_ = true;
			}
	};

	template<typename Position, class Particle, class Cluster, class World>
	class gl_visitor_3d : gl_visitor<Position, Particle, Cluster, World>
	{
		friend class gl_visitor<Position, Particle, Cluster, World>;

		public:
			void visit(particle<Position> * particle)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glPushMatrix();
				glTranslatef(particle->position.to_array()[0], particle->position.to_array()[1], particle->position.to_array()[2]);
				glCallList(cube_list_);
				glPopMatrix();
			}

			void visit(cluster<Position> * cluster)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT, GL_LINE);
				glBegin(GL_LINE_STRIP);
				glPushMatrix();
				glTranslatef(cluster->get_center().to_array()[0], cluster->get_center().to_array()[1], cluster->get_center().to_array()[2]);
				gluSphere(quadric_, cluster->get_radius(), this->tesselation, this->tesselation);
				glPopMatrix();
				glEnd();
				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_LIGHTING);

				glColor3f(1.0f, 0.0f, 0.0f);
				for(unsigned n = 0; n < cluster->get_particles().size(); ++n)
				{
					glPushMatrix();
					glTranslatef(cluster->get_particles()[n]->position.to_array()[0], cluster->get_particles()[n]->position.to_array()[1], cluster->get_particles()[n]->position.to_array()[2]);
					glCallList(cube_list_);
					glPopMatrix();
				}
			}

			void visit(World * world)
			{
				gl_visitor<Position, Particle, Cluster, World>::visit(world);

				if(this->view_changed_)
				{
					glViewport(0, 0, this->width_, this->height_);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					gluPerspective(90.0f, (float)this->width_ / this->height_, 1.0f, 1000.0f);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					glTranslatef(0, 0, -distance_);
					glRotatef(angle_, 0.0f, 1.0f, 0.0f);
					this->view_changed_ = false;
				}
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

		private:
			float distance_, angle_;
			int cube_list_;
			GLUquadricObj * quadric_;

			gl_visitor_3d() : distance_(100.0f), angle_(0), quadric_(gluNewQuadric())
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
						glNormal3f(0.0f, 0.0f, -1.0f); //back
						glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f); 
						glNormal3f(0.0f, 1.0f, 0.0f); //top
						glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, -0.5f); 
						glNormal3f(0.0f, 0.0f, 1.0f); //front
						glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); 
						glNormal3f(0.0f, -1.0f, 0.0f); //bottom
						glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
						glNormal3f(-1.0f, 0.0f, 0.0f); //left
						glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
						glNormal3f(1.0f, 0.0f, 0.0f); //right
						glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
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
	};
}

#endif
