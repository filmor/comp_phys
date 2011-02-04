#ifndef TRIVIAL_BATH_HPP
#define TRIVIAL_BATH_HPP

#include <vector>
#include <array>
#include <iostream>

#include "print.hpp"
#include "position.hpp"
#include "particle.hpp"
#include "cluster.hpp"

namespace trivial
{

	template<class Particle, int Size, unsigned FreeParticles>
	class uniform_bath
	{
    public:
        typedef typename Particle::position_type position_type;
        typedef cluster<Particle> cluster_type;
        typedef Particle particle_type;

        // TODO: Use random number generator from world
        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters) 
        {
            for (unsigned n = particles.size(); n < FreeParticles; ++n)
            {
                position_type pos;
                for (bool collision = true; collision;)
                {
                    for (unsigned m = 0; m < position_type::dimension; ++m)
                        pos += get_unit_vector<position_type> (m)
                               * ((rand() % Size) - Size / 2); //TODO generator

                    collision = false;

                    for (unsigned m = 0; m < particles.size(); ++m)
                        if (particles[m].position == pos)
                        {
                            collision = true;
                            break;
                        }
                    
                    if (!collision)
                        for (unsigned m = 0; m < clusters.size(); ++m)
                            if (clusters[m].has_particle_at(pos))
                            {
                                collision = true;
                                break;
                            }
                }
                particles.push_back(Particle(pos));
            }

        }
	};

	template<class Particle, int Size, unsigned FreeParticles>
	class diffusion_limited_bath
	{
    public:
        typedef typename Particle::position_type position_type;
        typedef cluster<Particle> cluster_type;
        typedef Particle particle_type;

        diffusion_limited_bath() : seeded_(false) {}

        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters) 
        {
            if(!seeded_)
            {
                position_type p;
                clusters.push_back(cluster_type());
                clusters.back().add_particle(particle_type(p));
                p += get_unit_vector<position_type>(0);
                clusters.back().add_particle(particle_type(p));
                seeded_ = true;
            }

            const position_type& center = clusters[0].get_center();
            const float radius = clusters[0].get_radius();

            int create = FreeParticles;
            for(unsigned n = 0; n < particles.size(); ++n)
                if(abs(particles[n].position - center)
                        > std::max(2 * radius, radius + 10))
                {
                    particles.erase(particles.begin() + n);
                    --n;
                }
                else
                {
                    --create;
                }
        
            while(create-- > 0)	
            {
                position_type pos;
                for(bool collision = true; collision;)
                {
                    std::array<float, position_type::dimension> e;
                    float mod = 0;
                    for(unsigned m = 0; m < e.size(); ++m)
                    {
                        e[m] = 2 * (float)rand() / RAND_MAX - 1;
                        //TODO normal distribution
                        mod += e[m] * e[m];
                    }
                    mod = sqrt(mod);
                    for(unsigned m = 0; m < position_type::dimension; ++m)
                        pos += get_unit_vector<position_type>(m)
                             * (e[m] * (radius + 5) / mod);
                    pos += center;

                    collision = false;
                    for(unsigned m = 0; m < particles.size(); ++m)
                        if(particles[m].position == pos)
                        {
                            collision = true;
                            break;
                        }
                    
                    if(!collision)
                        for(unsigned m = 0; m < clusters.size(); ++m)
                            if(clusters[m].has_particle_at(pos))
                            {
                                collision = true;
                                break;
                            }
                }
                particles.push_back(particle_type(pos));
            }

        }

    private:
		bool seeded_;
	};

	template<class Particle, int Size, unsigned Particles>
	class static_bath
	{
    public:
        typedef typename Particle::position_type position_type;
        typedef cluster<Particle> cluster_type;
        typedef Particle particle_type;

        static_bath() : done_(false) {}

        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type> & clusters) 
        {
            if(done_)
                return;
            uniform_bath<Particle, Size, Particles>().step(particles, clusters);
            done_ = true;
        }

    private:
		bool done_;
	};
}

#endif

