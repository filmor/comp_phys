#ifndef TRIVIAL_INTERACTION_HPP
#define TRIVIAL_INTERACTION_HPP

#include "vector.hpp"

namespace trivial
{

    template <typename Position>
    class standard_interaction
    {
    public:
        standard_interaction () : merge_(false) {}

        template <typename Object, typename Rng>
        void move (Object& obj, Rng& gen)
        {
            static std::uniform_int_distribution<unsigned> dist
                (0, Position::dimension * 2);

            const auto index = dist(gen);
            
            obj.move((index % 2 == 1 ? 1 : -1) * 
                        get_unit_vector<Position>(index / 2));
        }

        template <typename Rng>
        bool merge (Rng& gen)
        {
            if (merge_)
            {
                merge_ = false;
                return true;
            }
            else
                return false;
        }

        void set_merge() { merge_ = true; }

    private:
        bool merge_;
    };

    template <typename Position>
    class weighted_interaction
    {
    public:
        weighted_interaction () : merge_(0.0f)
        {
            for (int i = 0; i < 2 * Position::dimension; ++i)
                movement_ = 1.0;
        }

        template <typename Object, typename Rng>
        float move (Object& obj, Rng& rng)
        {
            for (int i = 1; i < 2 * Position::dimension; ++i)
                movement_[i] += movement_[i-1];

            std::uniform_real_distribution<float> dist
                    (movement_[0], movement_[2 * Position::dimension - 1] +
                                   movement_[0]);

            auto iter = std::lower_bound(movement_.begin(), movement_.end(),
                                         dist(rng));

            int i = iter - movement_.begin();

            assert(i >= 0 && i < 2 * Position::dimension - 1);

            return (i % 2 == 1 ? 1 : -1) * 
                    get_unit_vector<Position>(i / 2);
        }

    private:
        float merge_;
        std::array<float, 2 * Position::dimension> movement_;
    };

}

#endif
