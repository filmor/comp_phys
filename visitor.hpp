#ifndef TRIVIAL_VISITOR_HPP
#define TRIVIAL_VISITOR_HPP

namespace trivial
{
	template<typename Position>
	class particle;
	template<typename Position>
	class cluster;

	template<class Visited>
	class visitor
	{
		public:
			virtual void visit(Visited * visisted) = 0;
			virtual ~visitor() {}
	};

	template<typename Position, class World>
	class omni_visitor : public visitor<particle<Position>>, public visitor<cluster<Position>>, public visitor<World>
	{
		public:
			virtual ~omni_visitor() {}
	};

}

#endif
