#include "Spring.h"

#include "Blueberry\Core\Time.h"

namespace Blueberry
{
	Spring::Spring(const Vector3& restPosition, float lerpSpeed) : m_RestPosition(restPosition), m_LerpSpeed(lerpSpeed)
	{
	}

	Spring::Spring(const Vector3& stiffness, const Vector3& damping, float lerpSpeed) : m_Stiffness(stiffness), m_Damping(damping), m_LerpSpeed(lerpSpeed)
	{
	}

	const Vector3& Spring::GetPosition()
	{
		return m_LerpedPosition;
	}

	void Spring::Reset()
	{
		m_Position = {};
		m_Velocity = {};

		for (int i = 0; i < 100; i++)
		{
			m_DistributedForce[i] = {};
		}
	}

	void Spring::Adjust(const Vector3& stiffness, const Vector3& damping)
	{
		m_Stiffness = stiffness;
		m_Damping = damping;
	}

	void Spring::AddForce(const Vector3& force)
	{
		m_Velocity += force;
		UpdatePosition();
	}

	void Spring::AddForce(const Vector3& force, const uint32_t& distribution)
	{
		if (distribution > 1)
		{
			AddDistributedForce(force, distribution);
		}
		else
		{
			AddForce(force);
		}
	}

	void Spring::AddDistributedForce(const Vector3& force, const uint32_t& distribution)
	{
		uint32_t clampedDistribution = std::clamp(distribution, 1u, 100u);
		Vector3 distributedForce = force / clampedDistribution;
		AddForce(distributedForce);
		for (int i = 0; i < clampedDistribution - 1; i++)
		{
			m_DistributedForce[i] += distributedForce;
		}
	}

	void Spring::FixedUpdate()
	{
		static Vector3 empty = {};
		if (m_DistributedForce[0] != empty)
		{
			AddForce(m_DistributedForce[0]);
			for (int i = 0; i < 100; i++)
			{
				m_DistributedForce[i] = i < 99 ? m_DistributedForce[i + 1] : empty;
				if (m_DistributedForce[i] == empty)
				{
					break;
				}
			}
		}

		UpdateSpring();
		UpdatePosition();
	}

	void Spring::Update()
	{
		float deltaTime = Time::GetDeltaTime();
		if (m_LerpSpeed > 0.0f)
		{
			m_LerpedPosition = Vector3::Lerp(m_LerpedPosition, m_Position, deltaTime * m_LerpSpeed);
		}
		else
		{
			m_LerpedPosition = m_Position;
		}
	}

	void Spring::UpdateSpring()
	{
		m_Velocity += (m_RestPosition - m_Position) * m_Stiffness;
		m_Velocity *= Vector3(1, 1, 1) - m_Damping;
	}

	void Spring::UpdatePosition()
	{
		m_Position += m_Velocity * (50.0f * Time::GetFixedDeltaTime());
	}
}
