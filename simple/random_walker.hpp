#ifndef SIMPLE_RANDOM_WALKER_HPP
#define SIMPLE_RANDOM_WALKER_HPP

#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_smallint.hpp>

#include <iostream>

namespace simple
{

    /// Iterator-Interface!
    template <class Lattice, typename RandomNumberGenerator>
    class random_walker
    {
    public:
        typedef boost::uniform_smallint<> distribution_type;
        typedef typename Lattice::node_type node_type;

        random_walker (RandomNumberGenerator gen)
            : rand_(gen, distribution_type(1, Lattice::directions))
        {}

        /// TODO: unsigned step_size sollte auch float sein.
        ///       -> Metafunktion (step_size_type)
        void advance(unsigned step_count, unsigned step_size = 1)
        {
            for (unsigned int i = 0; i < step_count; ++i)
                step(step_size);
        }

        void step(unsigned step_size = 1)
        {
            node_.step(rand_(), step_size);   
        }

        node_type const& get_node () const { return node_; }
        void set_node (node_type const& node) { node_ = node; }

    private:
        boost::variate_generator<RandomNumberGenerator, distribution_type>
            rand_;
        node_type node_;
    };

    /// R-Value reference?
    template <typename Rng, typename Lattice>
    random_walker<Lattice, Rng> make_random_walker(Rng gen, Lattice const&)
    {
        return random_walker<Lattice, Rng>(gen);
    }

}

#endif
