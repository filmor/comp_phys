#ifndef TRIVIAL_TUBE_UPDATER_HPP
#define TRIVIAL_TUBE_UPDATER_HPP

#include <vector>
#include <random>

#include "../print.hpp"

namespace trivial
{
namespace tube
{

    template <class Particle, class Cluster, int Inlet, int Outlet, unsigned Radius, unsigned Rate, unsigned Spacing = 1>
    class uniform_updater
    {
    public:
        typedef typename Particle::position_type position_type;
        typedef Cluster cluster_type;
        typedef Particle particle_type;

        uniform_updater() : spawn_dist_(-Radius, Radius), wait_(1), entered_(0), passed_(0), step_(0)
        { assert(Inlet < Outlet); }

        template<class RandomNumberGenerator>
        void operator()(std::vector<particle_type>& particles,
                  std::vector<cluster_type>& clusters, RandomNumberGenerator& rng) 
        {
            for(unsigned n = 0; n < particles.size(); ++n)
                if(particles[n].position[0] < Inlet)
                {
                    --entered_;
                    particles.erase(particles.begin() + n);
                }
                else if(particles[n].position[0] > Outlet)
                {
                    ++passed_;
                    particles.erase(particles.begin() + n);
                }
            for(unsigned n = 0; n < clusters.size(); ++n)
                if(clusters[n].get_center()[0] < Inlet)
                {
                    entered_ -= clusters[n].get_size();
                    clusters.erase(clusters.begin() + n);
                }
                else if(clusters[n].get_center()[0] > Outlet)
                {
                    passed_ += clusters[n].get_size();
                    clusters.erase(clusters.begin() + n);
                }
            
            if(--wait_ == 0)
            {
                position_type pos;
                pos[0] = Inlet;
                for(unsigned n = 1; n < position_type::dimension; ++n)
                    pos[n] = (spawn_dist_(rng) / Spacing) * Spacing;

                bool collision = false;
                for(unsigned n = 0; n < particles.size(); ++n)
                    if(particles[n].position == pos)
                    {
                        collision = true;
                        break;
                    }
                if(!collision)
                    for(unsigned n = 0; n < clusters.size(); ++n)
                        if(clusters[n].has_particle_at(pos))
                        {
                            collision = true;
                            break;
                        }

                if(!collision)
                {
                    particles.push_back(particle_type(pos));
                    ++entered_;
                }

                wait_ = Rate;
            }

            if(++step_ % (100 * Rate) == 0)
            {
                print((float)passed_/(100 * Rate));
                passed_ = 0;
            }
        }

        private:
            std::uniform_int_distribution<int> spawn_dist_;
            unsigned wait_, entered_, passed_, step_;
    };

}
}

#endif

