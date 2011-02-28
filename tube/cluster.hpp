#ifndef TRIVIAL_TUBE_CLUSTER_HPP
#define TRIVIAL_TUBE_CLUSTER_HPP

#include <vector>

#include "../meakin/cluster.hpp"
#include "../interaction.hpp"

namespace trivial
{
namespace tube
{

    template <typename Particle>
    class flowing_cluster : public meakin::massive_moving_cluster<Particle>
    {
    public:
        flowing_cluster() : wait_(1) {}

        void move (typename Particle::position_type::vector_type const& v)
        {
            if(--wait_ == 0)
            //TODO should be done with probabilities once interactions are implented
            {
                ++meakin::static_cluster<Particle>::cube_center_[0];
                ++meakin::static_cluster<Particle>::ball_center_[0];
                wait_ = Particle::slowness;
            }
            else meakin::massive_moving_cluster<Particle>::move(v);
        }
    private:
        unsigned wait_;
    };

}
}

#endif
