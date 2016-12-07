//#include "BoundingSphere.h"
//#include <glm/gtx/norm.hpp>
//#include "D:\Napier\Year 3\Physics-Based Animations\new\praticals\6_rigid_body\game.h"
//#include "physics.h"
//#include "cPhysicsComponents.h"
//#include <phys_utils.h>
//
//IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingSphere & other)
//{
//	float radiusDistance = m_radius + other.m_radius;
//	float centerDistance = glm::length(other.GetCenter() - m_center);
//
//	// If the distance from each sphere together is less than the radius of one sphere they are intersecting
//	if (centerDistance < radiusDistance)
//	{
//		return IntersectData(true, centerDistance - radiusDistance);
//	}
//	else
//	{
//		return IntersectData(false, centerDistance - radiusDistance);
//	}
//}
//
//IntersectData BoundingSphere::IntersectPlane(const Entity& other)
//{
//	//float pointPlane = "plane equation";
//	//return IntersectData();
//}
//
