#pragma once

#include "Blueberry\Core\Base.h"

namespace Blueberry
{
	class Transform;

	class Spring
	{
	public:
		Spring() = default;
		Spring(const Vector3& restPosition, float lerpSpeed);
		Spring(const Vector3& stiffness, const Vector3& damping, float lerpSpeed);

		const Vector3& GetPosition();

		void Reset();
		void Adjust(const Vector3& stiffness, const Vector3& damping);
		void AddForce(const Vector3& force);
		void AddForce(const Vector3& force, const uint32_t& distribution);
		void AddDistributedForce(const Vector3& force, const uint32_t& distribution);
		void FixedUpdate();
		void Update();

	private:
		void UpdateSpring();
		void UpdatePosition();

	private:
		Vector3 m_Stiffness;
		Vector3 m_Damping;
		float m_LerpSpeed;

		Vector3 m_RestPosition;
		Vector3 m_Position;
		Vector3 m_LerpedPosition;
		Vector3 m_Velocity;
		Vector3 m_DistributedForce[100];
	};
}