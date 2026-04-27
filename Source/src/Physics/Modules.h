#pragma once

#include "Blueberry\Core\Object.h"

namespace Blueberry
{
	class OffsetModuleData : public Data
	{
		DATA_DECLARATION(OffsetModuleData)

	public:
		OffsetModuleData() = default;
		OffsetModuleData(bool enabled, const Vector3& positionOffset, const Vector3& rotationOffset);

		bool IsEnabled() const;
		const Vector3& GetPositionOffset() const;
		const Vector3& GetRotationOffset() const;

	private:
		bool m_IsEnabled;
		Vector3 m_PositionOffset;
		Vector3 m_RotationOffset;
	};

	class BobModuleData : public Data
	{
		DATA_DECLARATION(BobModuleData)

	public:
		BobModuleData() = default;
		BobModuleData(bool enabled, float bobSpeed, const Vector3& positionAmplitude, const Vector3& rotationAmplitude);

		bool IsEnabled() const;
		float GetBobSpeed() const;
		const Vector3& GetPositionAmplitude() const;
		const Vector3& GetRotationAmplitude() const;

	private:
		bool m_IsEnabled;
		float m_BobSpeed;
		Vector3 m_PositionAmplitude;
		Vector3 m_RotationAmplitude;
	};

	class NoiseModuleData : public Data
	{
		DATA_DECLARATION(NoiseModuleData)

	public:
		NoiseModuleData() = default;
		NoiseModuleData(bool enabled, float noiseSpeed, const Vector3& positionAmplitude, const Vector3& rotationAmplitude);

		bool IsEnabled() const;
		float GetMaxJitter() const;
		float GetNoiseSpeed() const;
		const Vector3& GetPositionAmplitude() const;
		const Vector3& GetRotationAmplitude() const;

	private:
		bool m_IsEnabled;
		float m_MaxJitter;
		float m_NoiseSpeed;
		Vector3 m_PositionAmplitude;
		Vector3 m_RotationAmplitude;
	};

	class SwayModuleData : public Data
	{
		DATA_DECLARATION(SwayModuleData)

	public:
		SwayModuleData() = default;
		SwayModuleData(float lookInputMultiplier, float maxLookInput, float aimMultiplier, const Vector3& lookPositionSway, const Vector3& lookRotationSway, const Vector3& strafePositionSway, const Vector3& strafeRotationSway);

		float GetLookInputMultiplier() const;
		float GetMaxLookInput() const;
		float GetAimMultiplier() const;
		const Vector3& GetLookPositionSway() const;
		const Vector3& GetLookRotationSway() const;
		const Vector3& GetStrafePositionSway() const;
		const Vector3& GetStrafeRotationSway() const;

	private:
		float m_LookInputMultiplier;
		float m_MaxLookInput;
		float m_AimMultiplier;
		Vector3 m_LookPositionSway;
		Vector3 m_LookRotationSway;
		Vector3 m_StrafePositionSway;
		Vector3 m_StrafeRotationSway;
	};

	class StepForceModuleData : public Data
	{
		DATA_DECLARATION(StepForceModuleData)

	public:
		bool IsEnabled() const;
		const Vector3& GetPositionForce() const;
		const uint32_t& GetPositionDistribution() const;
		const Vector3& GetRotationForce() const;
		const uint32_t& GetRotationDistribution() const;

	private:
		bool m_IsEnabled;
		Vector3 m_PositionForce;
		uint32_t m_PositionDistribution;
		Vector3 m_RotationForce;
		uint32_t m_RotationDistribution;
	};

	class JumpModuleData : public Data
	{
		DATA_DECLARATION(JumpModuleData)

	public:
		JumpModuleData() = default;
		JumpModuleData(bool isEnabled, const Vector3& positionForce, const uint32_t& positionDistribution, const Vector3& rotationForce, const uint32_t& rotationDistribution);

		bool IsEnabled() const;
		const Vector3& GetPositionForce() const;
		const uint32_t& GetPositionDistribution() const;
		const Vector3& GetRotationForce() const;
		const uint32_t& GetRotationDistribution() const;

	private:
		bool m_IsEnabled;
		Vector3 m_PositionForce;
		uint32_t m_PositionDistribution;
		Vector3 m_RotationForce;
		uint32_t m_RotationDistribution;
	};
}