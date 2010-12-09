#ifndef SIMPLE_CLUSTER_HPP
#define SIMPLE_CLUSTER_HPP

#include <vector>
#include <boost/foreach.hpp>

namespace simple
{

    template <typename Lattice>
    class cluster
    {
    public:
        typedef typename Lattice::node_type node_type;

        cluster(std::initializer_list<node_type> nodes)
            : nodes_(nodes)
        {}

        inline friend
        float distance (node_type const& node, cluster const& cl)
        {
            return distance(cl, node);
        }

        inline friend
        float distance (cluster const& cl, node_type const& node)
        {
            unsigned res = distance(cl.center(), node);

            BOOST_FOREACH(node_type const& i, cl.nodes_)
            {
                res = std::min(distance(i, node), res);
            }

            return res;
        }

        void add_node(node_type node)
        {
            nodes_.push_back(node);
        }

        node_type center () const
        {
            if (nodes_.size() > 0)
                return nodes_[0];
            else
                return node_type();
        }

        bool contains (node_type const& n)
        {
            for (int i = 0; i < nodes_.size(); ++i)
            {
                if (n == nodes_[i])
                    return true;
            }
            return false;
        }

        std::vector<node_type> const& get_nodes() const { return nodes_; }

    private:
        std::vector<node_type> nodes_;
    };

}

#endif
