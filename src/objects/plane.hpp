#ifndef __PLANE_HPP__
#define __PLANE_HPP__

#include "base_object.hpp"
#include "geometry/plane_geometry.hpp"

namespace RT_ISICG
{
	class Plane : public BaseObject
	{
	  public:
		Plane()          = delete;
		virtual ~Plane() = default;

		Plane(const std::string& p_name, const Vec3f& p_point, const Vec3f& p_normal)
			: BaseObject(p_name), _geometry(p_point, p_normal) { }

		virtual bool intersect( const Ray & p_ray,
								const float p_tMin,
								const float p_tMax,
								HitRecord & hitRecord ) const override;

		virtual bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;
		
		virtual inline bool isPlane() const override { return true; }

	  private:
		PlaneGeometry _geometry;

	};
}

#endif // !__PLANE_HPP__
