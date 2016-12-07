//////#pragma once
//////
//#include <glm/glm.hpp>
//#include "IntersectData.h"
//#include "graphics_framework.h"
//
//class BoundingSphere
//{
//public: 
//	BoundingSphere(const glm::vec3& center, float radius) : m_center(center), m_radius(radius)
//	{
//
//	}
//
//	IntersectData IntersectBoundingSphere(const BoundingSphere& other);
//
//	IntersectData IntersectPlane(const Entity& other);
//
//	// Getters and setters for private class attributes
//	inline const glm::vec3& GetCenter() const 
//	{
//		return m_center;
//	}
//
//	inline float GetRadius() const
//	{
//		return m_radius;
//	}
//
//
//private:
//	const glm::vec3 m_center;
//	const float m_radius;
//};