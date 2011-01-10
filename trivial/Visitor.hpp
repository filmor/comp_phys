#ifndef TRIVIAL_VISITOR_HPP
#define TRIVIAL_VISITOR_HPP

#include <GL/glfw.h>
#include <iostream>

namespace trivial
{
	using std::cout;
	using std::endl;

	template<class TOPOLOGY>
	class Particle;
	template<class TOPOLOGY>
	class Cluster;

	template<class VISITED>
	class Visitor
	{
		public:
			virtual void visit(VISITED * visisted) = 0;
			virtual ~Visitor() {}
	};

	template<class TOPOLOGY, class WORLD>
	class OmniVisitor : Visitor<Particle<TOPOLOGY>>, Visitor<Cluster<TOPOLOGY>>, Visitor<WORLD>
	{
		public:
			virtual ~OmniVisitor() {}
	};

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class WORLD>
	class GLVisitor : public OmniVisitor<TOPOLOGY, WORLD>
	{
		static float POINT_SIZE;
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

				POINT_SIZE = point_size;

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

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class WORLD>
	float GLVisitor<TOPOLOGY, PARTICLE, CLUSTER, WORLD>::POINT_SIZE = 1;

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class WORLD, bool WORLDONLY>
	class PopulationVisitor : public OmniVisitor<TOPOLOGY, WORLD>
	{

		void visit(Particle<TOPOLOGY> * particle)
		{
			if(WORLDONLY)
				return;

			cout << "particle [" << particle << "] at (";
			for(unsigned n = 0; n < TOPOLOGY::dimensions - 1; ++n)
				cout << particle->position[n] << ",";
			cout << particle->position[TOPOLOGY::dimensions - 1] << ")" << endl;
		}

		void visit(Cluster<TOPOLOGY> * cluster)
		{
			if(WORLDONLY)
				return;

			cout << "cluster [" << cluster << "] at (";
			for(unsigned n = 0; n < TOPOLOGY::dimensions - 1; ++n)
				cout << cluster->get_bounds().center[n] << ",";
			cout << cluster->get_bounds().center[TOPOLOGY::dimensions - 1] << ") radius: " << cluster->get_bounds().radius << ", particles: " << ((CLUSTER *)cluster)->get_particles().size() << endl;

			for(unsigned n = 0; n < cluster->get_particles().size(); ++n)
			{
				cout << "\t";
				visit(cluster->get_particles()[n]);
			}
		}

		void visit(WORLD * world)
		{
			unsigned sum = 0;
			for(unsigned n = 0; n < world->get_clusters().size(); ++n)
				sum += world->get_clusters()[n]->get_particles().size();
			cout << "Currently " << world->get_particles().size() << " free particle(s) and " << sum << " in  " << world->get_clusters().size() << " cluster(s)." << endl;
		}
	};
}

#endif

