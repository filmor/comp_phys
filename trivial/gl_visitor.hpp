#ifndef TRIVIAL_GL_VISITOR_HPP
#define TRIVIAL_GL_VISITOR_HPP

#include <GL/glfw.h>

#include "visitor.hpp"

#define POINT_SIZE 1.0

namespace trivial
{
	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class WORLD>
	class GLVisitor : public OmniVisitor<TOPOLOGY, WORLD>
	{
		const bool LIMIT_FPS;

		static void GLFWCALL reshape(int width, int height)
		{
			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, width / POINT_SIZE, 0, height / POINT_SIZE, -1, 1);
			glMatrixMode(GL_MODELVIEW);
		}

		public:
			GLVisitor(unsigned width = 400, unsigned height = 400, float point_size = 2, bool limit_fps = false) : LIMIT_FPS(limit_fps)
			{
				if(TOPOLOGY::dimensions != 2)
					throw "not supported yet";

				glfwInit();
				glfwOpenWindow(width, height, 8, 8, 8, 8, 8, 0, GLFW_WINDOW);
				glfwSetWindowSizeCallback(&reshape);

				glPointSize(POINT_SIZE);
			}

			~GLVisitor()
			{
				glfwTerminate();
			}

			void visit(Particle<TOPOLOGY> * particle)
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_POINTS);
				glVertex2i(particle->position[0], particle->position[1]);
				glEnd();
			}

			void visit(Cluster<TOPOLOGY> * cluster)
			{
				for(unsigned n = 0; n < cluster->get_particles().size(); ++n)
				{
					glColor3f(1.0f, 0.0f, 0.0f);
					glBegin(GL_POINTS);
					glVertex2i(cluster->get_particles()[n]->position[0], cluster->get_particles()[n]->position[1]);
					glEnd();
				}
			}

			void visit(WORLD * world)
			{
				glFlush();
				glfwSwapBuffers();
				if(LIMIT_FPS)
					glfwSleep(0.1);
				glClear(GL_COLOR_BUFFER_BIT);
			}
	};

}

#endif
