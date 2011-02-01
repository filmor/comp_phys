#ifndef TRIVIAL_GL_VISITOR_HPP
#define TRIVIAL_GL_VISITOR_HPP

#include <GL/glfw.h>

#include "visitor.hpp"

#define CLUSTER_CIRCLE_SEGMENTS 36

namespace trivial
{
    // TODO: PointSize shouldn't have to be static!
	template<class World, int PointSize>
	class gl_visitor : public const_world_visitor<World>
	{
		const bool limit_fps_;

		static void GLFWCALL reshape(int width, int height)
		{
			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-width / (2 * PointSize),
                    width / (2 * PointSize),
                    -height / (2 * PointSize),
                    height / (2 * PointSize),
                    -1, 1);
			glMatrixMode(GL_MODELVIEW);
		}

		public:
			gl_visitor(unsigned width = 400, unsigned height = 400,
                       bool limit_fps = false)
                : limit_fps_(limit_fps)
			{
				if(World::position_type::dimension != 2)
					throw "not supported yet";

				glfwInit();
				glfwOpenWindow(width, height, 8, 8, 8, 8, 8, 0, GLFW_WINDOW);
				glfwSetWindowSizeCallback(&reshape);

				glPointSize(PointSize);
			}

			~gl_visitor()
			{
				glfwTerminate();
			}

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
				for(unsigned n = 0; n <= CLUSTER_CIRCLE_SEGMENTS; ++n)
					glVertex2f(
                          cluster.get_center()[0]
                          + cluster.get_radius()
                            * sin(n * (2 * M_PI / CLUSTER_CIRCLE_SEGMENTS)),
                          cluster.get_center()[1]
                          + cluster.get_radius()
                            * cos(n * (2 * M_PI / CLUSTER_CIRCLE_SEGMENTS))
                        ); 
				glEnd();

                // TODO: MultiIndex: We need a vector of positions of particles
                //       as well as a map of positions to particles
                /*
				for(unsigned n = 0; n < cluster.get_particles().size(); ++n)
				{
					glColor3f(1.0f, 0.0f, 0.0f);
					glBegin(GL_POINTS);
					// glVertex2i(cluster->get_particles()[n]->position.to_array()[0], cluster->get_particles()[n]->position.to_array()[1]);
					glEnd();
				} */
			}

			void visit(World const& world)
			{
				glFlush();
				glfwSwapBuffers();
				if(limit_fps_)
					glfwSleep(0.1);
				glClear(GL_COLOR_BUFFER_BIT);
			}
	};

}

#endif
