#ifndef TRIVIAL_BATH_HPP
#define TRIVIAL_BATH_HPP

#include <vector>
#include <random>

#include "print.hpp"

namespace trivial
{
namespace meakin
{

    template <class Particle, class Cluster, int Size, unsigned FreeParticles>
    class uniform_bath
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        template<class RandomNumberGenerator>
        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters, RandomNumberGenerator & rng) 
        {
            for (unsigned n = particles.size(); n < FreeParticles; ++n)
            {
                position_type pos;
                for (bool collision = true; collision;)
                {
                    for (unsigned m = 0; m < position_type::dimension; ++m)
                        pos += get_unit_vector<position_type>(m) * ((float)rng() / rng.max() * Size - Size / 2); 

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

    template <class Particle, class Cluster, int Size, unsigned FreeParticles>
    class diffusion_limited_bath
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        diffusion_limited_bath() : seeded_(false) {}

        template<class RandomNumberGenerator>
        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters, RandomNumberGenerator & rng) 
        {
            if(!seeded_)
            {
                position_type p;
                clusters.push_back(cluster_type());
                clusters.back().add_particle(particle_type(p), rng);
                p += get_unit_vector<position_type>(0);
                clusters.back().add_particle(particle_type(p), rng);
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
                    typename position_type::float_vector_type e;
                    float mod = 0;
                    for(unsigned m = 0; m < e.size(); ++m)
                    {
                        e[m] = norm_dist_(rng);
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
        std::normal_distribution<float> norm_dist_;
        bool seeded_;
    };

    template<class Particle, class Cluster, int Size, unsigned Particles>
    class static_bath : public uniform_bath<Particle, Cluster, Size, Particles>
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        static_bath() : done_(false) {}

        template<class RandomNumberGenerator>
        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type> & clusters, RandomNumberGenerator & rng) 
        {
            if(done_)
                return;
            uniform_bath<Particle, Cluster, Size, Particles>::step(particles, clusters, rng);
            done_ = true;
        }

    private:
        bool done_;
    };

}
}

#endif

