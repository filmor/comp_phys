#ifndef TRIVIAL_BATH_HPP
#define TRIVIAL_BATH_HPP

#include <vector>
#include <array>
#include <iostream>

#include "position.hpp"
#include "particle.hpp"
#include "cluster.hpp"

namespace trivial
{
	template<typename Position, class Particle, class Cluster, class Bath>
	class world;

	//abc
	template<typename Position, class Particle, class Cluster>
	class bath
	{
		public:
			void step(std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters);
	};

	template<typename Position, class Particle, class Cluster, int Size, unsigned FreeParticles>
	class uniform_bath : public bath<Position, Particle, Cluster>
	{
		public:
			void step(std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters) 
			{
				for(unsigned n = particles.size(); n < FreeParticles; ++n)
				{
					Position pos;
					for(bool collision = true; collision;)
					{
						for(unsigned m = 0; m < Position::dimension; ++m)
							pos += Position::unit_vectors[m] * ((rand() % Size) - Size / 2); //TODO generator

						collision = false;
						for(unsigned m = 0; m < particles.size(); ++m)
							if(particles[m]->position == pos)
							{
								std::cout << "collision" << std::endl;
								collision = true;
								break;
							}
						
						if(!collision)
							for(unsigned m = 0; m < clusters.size(); ++m)
								if(clusters[m]->has_particle_at(pos))
								{
									std::cout << "collision" << std::endl;
									collision = true;
									break;
					
								}
					}
					particles.push_back(new Particle(pos));
				}

			}
	};

	template<typename Position, class Particle, class Cluster, int Size, unsigned FreeParticles>
	class diffusion_limited_bath : public bath<Position, Particle, Cluster>
	{
		bool seeded_;

		public:
			diffusion_limited_bath() : seeded_(false) {}

			void step(std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters) 
			{
				if(!seeded_)
				{
					Position p;
					clusters.push_back(new Cluster);
					clusters[0]->add_particle(new Particle(p));
					p += Position::unit_vectors[0];
					clusters[0]->add_particle(new Particle(p));
					seeded_ = true;
				}

				const Position & center = clusters[0]->get_center();
				const float radius = clusters[0]->get_radius();

				int create = FreeParticles;
				for(unsigned n = 0; n < particles.size(); ++n)
					if(abs(particles[n]->position - center) > std::max(2 * radius, radius + 10))
					{
						delete particles[n];
						particles.erase(particles.begin() + n);
						--n;
					}
					else --create;
			
				while(create-- > 0)	
				{
					Position pos;
					for(bool collision = true; collision;)
					{
						std::array<float, Position::dimension> e;
						float mod = 0;
						for(unsigned m = 0; m < Position::dimension; ++m)
						{
							e[m] = 2 * (float)rand() / RAND_MAX - 1;		//TODO normal distribution
							mod += e[m] * e[m];
						}
						mod = sqrt(mod);
						for(unsigned m = 0; m < Position::dimension; ++m)
							pos += Position::unit_vectors[m] * (e[m] * (radius + 5) / mod);
						pos += center;
						pos.round();

						collision = false;
						for(unsigned m = 0; m < particles.size(); ++m)
							if(particles[m]->position == pos)
							{
								std::cout << "collision" << std::endl;
								collision = true;
								break;
							}
						
						if(!collision)
							for(unsigned m = 0; m < clusters.size(); ++m)
								if(clusters[m]->has_particle_at(pos))
								{
									std::cout << "collision" << std::endl;
									collision = true;
									break;
					
								}
					}
					particles.push_back(new Particle(pos));
				}

			}
	};

	template<typename Position, class Particle, class Cluster, int Size, unsigned Particles>
	class static_bath : public bath<Position, Particle, Cluster>
	{
		bool done_;

		public:
			static_bath() : done_(false) {}

			void step(std::vector<particle<Position> *> & particles, std::vector<cluster<Position> *> & clusters) 
			{
				if(done_)
					return;
				uniform_bath<Position, Particle, Cluster, Size, Particles>().step(particles, clusters);
				done_ = true;
			}
	};
}

#endif

