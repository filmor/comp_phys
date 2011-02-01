#ifndef TRIVIAL_VISITOR_HPP
#define TRIVIAL_VISITOR_HPP

namespace trivial
{
	template<typename Position>
	class particle;
	template<typename Position>
	class cluster;

	template <class Visited>
    struct visitor
	{
        virtual void visit(Visited& visited) {}
        virtual ~visitor() {}
	};
    
    template <class Visited>
    struct const_visitor
    {
        virtual void visit(Visited const& visited) {}
        virtual ~const_visitor() {}
    };

#define TRIVIAL_DEFINE_VISITABLE(type) \
    void accept (visitor<type>& v) { v.visit(*this); } \
    void accept (const_visitor<type>& v) const { v.visit(*this); }

	template <class World>
	struct world_visitor : visitor<typename World::particle_type>
                         , visitor<typename World::cluster_type>
                         , visitor<World>
	{
	};

    template <class World>
    struct const_world_visitor : const_visitor<typename World::particle_type>
                               , const_visitor<typename World::cluster_type>
                               , const_visitor<World>
    {};

}

#endif
