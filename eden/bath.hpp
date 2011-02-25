#ifndef TRIVIAL_EDEN_BATH_HPP
#define TRIVIAL_EDEN_BATH_HPP

#include <vector>
#include <random>

#include "../print.hpp"

namespace trivial
{
namespace eden
{

    template <class Particle, class Cluster>
    class nearest_neighbours_bath
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        template<class RandomNumberGenerator>
        void step(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters, RandomNumberGenerator & rng) 
        {
            if(particles.size() > 0)
                return;

            if(clusters.size() == 0)
            {
                position_type p;
                clusters.push_back(cluster_type());
                clusters.back().add_particle(particle_type(p));
                p += get_unit_vector<position_type>(0);
                clusters.back().add_particle(particle_type(p));
            }   

            //chose neighbouring site with probability proportional to number of neighbours
            std::vector<position_type> neighbours;
            for(unsigned n = 0; n < clusters.back().get_size(); ++n)
                for(unsigned m = 0; m < 2 * position_type::dimension; ++m)
                {
                    position_type p = clusters.back().abs_position(clusters.back().get_particles()[n].position) + (m % 2 ? 1 : -1) * get_unit_vector<position_type>(m / 2);
                    if(!clusters.back().has_particle_at(p))
                        neighbours.push_back(p);
                }

            if(neighbours.size() == 0)
               print("no free neighbours!");
            else particles.push_back(particle_type(neighbours[std::uniform_int_distribution<unsigned>(0, neighbours.size() - 1)(rng)]));
        }
    };

}
}

#endif

