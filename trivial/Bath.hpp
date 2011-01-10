#ifndef TRIVIAL_BATH_HPP
#define TRIVIAL_BATH_HPP

#include <array>
#include <vector>
#include <iostream>

#include "Topology.hpp"
#include "Particle.hpp"
#include "Cluster.hpp"

namespace trivial
{
	template<class TOPOLOGY, class PARTICLE, class CLUSTER, class BATH>
	class World;

	//prototype. not implemented
	template<class TOPOLOGY, class PARTICLE, class CLUSTER>
	class Bath
	{
		public:
			typedef typename TOPOLOGY::position_type position_type;
			typedef typename TOPOLOGY::distance_type distance_type;
			typedef typename TOPOLOGY::neighbours_type neighbours_type;
			void step(std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters);
	};

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, unsigned SIZE, unsigned FREE_PARTICLES>
	class UniformBath : public Bath<TOPOLOGY, PARTICLE, CLUSTER>
	{
		public:
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::position_type position_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::distance_type distance_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::neighbours_type neighbours_type;

			void step(std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters) 
			{
				for(unsigned n = particles.size(); n < FREE_PARTICLES; ++n)
				{
					position_type pos;
					for(bool collision = true; collision;)
					{
						for(unsigned m = 0; m < TOPOLOGY::dimensions; ++m)
							pos[m] = rand() % SIZE;
						collision = false;
						for(unsigned m = 0; m < particles.size(); ++m)
							if(TOPOLOGY::intersect(particles[m]->position, pos))
							{
								std::cout << "collision" << endl;
								collision = true;
								break;
							}
						
						if(!collision)
							for(unsigned m = 0; m < clusters.size(); ++m)
								if(clusters[m]->has_particle_at(pos))
								{
									std::cout << "collision" << endl;
									collision = true;
									break;
					
								}
					}
					particles.push_back(new PARTICLE(pos));
				}

			}
	};

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, unsigned SIZE, unsigned FREE_PARTICLES>
	class DiffusionLimitedBath : public Bath<TOPOLOGY, PARTICLE, CLUSTER>
	{
		bool seeded_;

		public:
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::position_type position_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::distance_type distance_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::neighbours_type neighbours_type;

			DiffusionLimitedBath() : seeded_(false) {}

			void step(std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters) 
			{
				if(!seeded_)
				{
					position_type p;
					for(unsigned n = 0; n < TOPOLOGY::dimensions; ++n)
						p[n] = SIZE / 2;
					clusters.push_back(new CLUSTER);
					clusters[0]->add_particle(new PARTICLE(p));
					++p[0];
					clusters[0]->add_particle(new PARTICLE(p));
					seeded_ = true;
				}

				const position_type & center = clusters[0]->get_bounds().center;
				const distance_type & radius = clusters[0]->get_bounds().radius;

				int create = FREE_PARTICLES;
				for(unsigned n = 0; n < particles.size(); ++n)
					if(TOPOLOGY::dist(particles[n]->position, center) > /*2 **/ radius + 10)
					{
						delete particles[n];
						particles.erase(particles.begin() + n);
						--n;
					}
					else --create;
			
				while(create-- > 0)	
				{
					position_type pos;
					for(bool collision = true; collision;)
					{
						std::array<float, TOPOLOGY::dimensions> e;
						float mod = 0;
						for(unsigned m = 0; m < TOPOLOGY::dimensions; ++m)
						{
							e[m] = 2 * (float)rand() / RAND_MAX - 1;		//TODO normal distribution
							mod += e[m] * e[m];
						}
						mod = sqrt(mod);
						for(unsigned m = 0; m < TOPOLOGY::dimensions; ++m)
							pos[m] = center[m] + e[m] * (radius + 5 ) / mod;	

						collision = false;
						for(unsigned m = 0; m < particles.size(); ++m)
							if(TOPOLOGY::intersect(particles[m]->position, pos))
							{
								std::cout << "collision" << endl;
								collision = true;
								break;
							}
						
						if(!collision)
							for(unsigned m = 0; m < clusters.size(); ++m)
								if(clusters[m]->has_particle_at(pos))
								{
									std::cout << "collision" << endl;
									collision = true;
									break;
					
								}
					}
					particles.push_back(new PARTICLE(pos));
				}

			}
	};

	template<class TOPOLOGY, class PARTICLE, class CLUSTER, unsigned SIZE, unsigned PARTICLES>
	class StaticBath : public Bath<TOPOLOGY, PARTICLE, CLUSTER>
	{
		bool done_;

		public:
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::position_type position_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::distance_type distance_type;
			typedef typename Bath<TOPOLOGY, PARTICLE, CLUSTER>::neighbours_type neighbours_type;

			StaticBath() : done_(false) {}

			void step(std::vector<Particle<TOPOLOGY> *> & particles, std::vector<Cluster<TOPOLOGY> *> & clusters) 
			{
				if(done_)
					return;
				UniformBath<TOPOLOGY, PARTICLE, CLUSTER, SIZE, PARTICLES>().step(particles, clusters);
				done_ = true;
			}
	};
}

#endif

