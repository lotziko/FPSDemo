#include "Modules.h"

#include "Blueberry\Core\ClassDB.h"

namespace Blueberry
{
	DATA_DEFINITION(OffsetModuleData)
	{
		DEFINE_FIELD(OffsetModuleData, m_IsEnabled, BindingType::Bool, {})
		DEFINE_FIELD(OffsetModuleData, m_PositionOffset, BindingType::Vector3, {})
		DEFINE_FIELD(OffsetModuleData, m_RotationOffset, BindingType::Vector3, {})
	}

	DATA_DEFINITION(BobModuleData)
	{
		DEFINE_FIELD(BobModuleData, m_IsEnabled, BindingType::Bool, {})
		DEFINE_FIELD(BobModuleData, m_BobSpeed, BindingType::Float, {})
		DEFINE_FIELD(BobModuleData, m_PositionAmplitude, BindingType::Vector3, {})
		DEFINE_FIELD(BobModuleData, m_RotationAmplitude, BindingType::Vector3, {})
	}

	DATA_DEFINITION(NoiseModuleData)
	{
		DEFINE_FIELD(NoiseModuleData, m_IsEnabled, BindingType::Bool, {})
		DEFINE_FIELD(NoiseModuleData, m_MaxJitter, BindingType::Float, {})
		DEFINE_FIELD(NoiseModuleData, m_NoiseSpeed, BindingType::Float, {})
		DEFINE_FIELD(NoiseModuleData, m_PositionAmplitude, BindingType::Vector3, {})
		DEFINE_FIELD(NoiseModuleData, m_RotationAmplitude, BindingType::Vector3, {})
	}

	DATA_DEFINITION(SwayModuleData)
	{
		DEFINE_FIELD(SwayModuleData, m_LookInputMultiplier, BindingType::Float, {})
		DEFINE_FIELD(SwayModuleData, m_MaxLookInput, BindingType::Float, {})
		DEFINE_FIELD(SwayModuleData, m_AimMultiplier, BindingType::Float, {})
		DEFINE_FIELD(SwayModuleData, m_LookPositionSway, BindingType::Vector3, {})
		DEFINE_FIELD(SwayModuleData, m_LookRotationSway, BindingType::Vector3, {})
		DEFINE_FIELD(SwayModuleData, m_StrafePositionSway, BindingType::Vector3, {})
		DEFINE_FIELD(SwayModuleData, m_StrafeRotationSway, BindingType::Vector3, {})
	}

	DATA_DEFINITION(StepForceModuleData)
	{
		DEFINE_FIELD(StepForceModuleData, m_IsEnabled, BindingType::Bool, {})
		DEFINE_FIELD(StepForceModuleData, m_PositionForce, BindingType::Vector3, {})
		DEFINE_FIELD(StepForceModuleData, m_PositionDistribution, BindingType::Uint, {})
		DEFINE_FIELD(StepForceModuleData, m_RotationForce, BindingType::Vector3, {})
		DEFINE_FIELD(StepForceModuleData, m_RotationDistribution, BindingType::Uint, {})
	}

	OffsetModuleData::OffsetModuleData(bool enabled, const Vector3& positionOffset, const Vector3& rotationOffset) : m_IsEnabled(enabled), m_PositionOffset(positionOffset), m_RotationOffset(rotationOffset)
	{
	}

	bool OffsetModuleData::IsEnabled() const
	{
		return m_IsEnabled;
	}

	const Vector3& OffsetModuleData::GetPositionOffset() const
	{
		return m_PositionOffset;
	}

	const Vector3& OffsetModuleData::GetRotationOffset() const
	{
		return m_RotationOffset;
	}

	BobModuleData::BobModuleData(bool enabled, float bobSpeed, const Vector3& positionAmplitude, const Vector3& rotationAmplitude) : m_IsEnabled(enabled), m_BobSpeed(bobSpeed), m_PositionAmplitude(positionAmplitude), m_RotationAmplitude(rotationAmplitude)
	{
	}

	bool BobModuleData::IsEnabled() const
	{
		return m_IsEnabled;
	}

	float BobModuleData::GetBobSpeed() const
	{
		return m_BobSpeed;
	}

	const Vector3& BobModuleData::GetPositionAmplitude() const
	{
		return m_PositionAmplitude;
	}

	const Vector3& BobModuleData::GetRotationAmplitude() const
	{
		return m_RotationAmplitude;
	}

	NoiseModuleData::NoiseModuleData(bool enabled, float noiseSpeed, const Vector3& positionAmplitude, const Vector3& rotationAmplitude) : m_IsEnabled(enabled), m_NoiseSpeed(noiseSpeed), m_PositionAmplitude(positionAmplitude), m_RotationAmplitude(rotationAmplitude)
	{
	}

	bool NoiseModuleData::IsEnabled() const
	{
		return m_IsEnabled;
	}

	float NoiseModuleData::GetMaxJitter() const
	{
		return m_MaxJitter;
	}

	float NoiseModuleData::GetNoiseSpeed() const
	{
		return m_NoiseSpeed;
	}

	const Vector3& NoiseModuleData::GetPositionAmplitude() const
	{
		return m_PositionAmplitude;
	}

	const Vector3& NoiseModuleData::GetRotationAmplitude() const
	{
		return m_RotationAmplitude;
	}

	SwayModuleData::SwayModuleData(float lookInputMultiplier, float maxLookInput, float aimMultiplier, const Vector3& lookPositionSway, const Vector3& lookRotationSway, const Vector3& strafePositionSway, const Vector3& strafeRotationSway) : m_LookInputMultiplier(lookInputMultiplier), m_MaxLookInput(maxLookInput), m_AimMultiplier(aimMultiplier), m_LookPositionSway(lookPositionSway), m_LookRotationSway(lookRotationSway), m_StrafePositionSway(strafePositionSway), m_StrafeRotationSway(strafeRotationSway)
	{
	}

	float SwayModuleData::GetLookInputMultiplier() const
	{
		return m_LookInputMultiplier;
	}

	float SwayModuleData::GetMaxLookInput() const
	{
		return m_MaxLookInput;
	}

	float SwayModuleData::GetAimMultiplier() const
	{
		return m_AimMultiplier;
	}

	const Vector3& SwayModuleData::GetLookPositionSway() const
	{
		return m_LookPositionSway;
	}

	const Vector3& SwayModuleData::GetLookRotationSway() const
	{
		return m_LookRotationSway;
	}

	const Vector3& SwayModuleData::GetStrafePositionSway() const
	{
		return m_StrafePositionSway;
	}

	const Vector3& SwayModuleData::GetStrafeRotationSway() const
	{
		return m_StrafeRotationSway;
	}

	bool StepForceModuleData::IsEnabled() const
	{
		return m_IsEnabled;
	}

	const Vector3& StepForceModuleData::GetPositionForce() const
	{
		return m_PositionForce;
	}

	const uint32_t& StepForceModuleData::GetPositionDistribution() const
	{
		return m_PositionDistribution;
	}

	const Vector3& StepForceModuleData::GetRotationForce() const
	{
		return m_RotationForce;
	}

	const uint32_t& StepForceModuleData::GetRotationDistribution() const
	{
		return m_RotationDistribution;
	}

	JumpModuleData::JumpModuleData(bool isEnabled, const Vector3& positionForce, const uint32_t& positionDistribution, const Vector3& rotationForce, const uint32_t& rotationDistribution) : m_IsEnabled(isEnabled), m_PositionForce(positionForce), m_PositionDistribution(positionDistribution), m_RotationForce(rotationForce), m_RotationDistribution(rotationDistribution)
	{
	}

	bool JumpModuleData::IsEnabled() const
	{
		return m_IsEnabled;
	}

	const Vector3& JumpModuleData::GetPositionForce() const
	{
		return m_PositionForce;
	}

	const uint32_t& JumpModuleData::GetPositionDistribution() const
	{
		return m_PositionDistribution;
	}

	const Vector3& JumpModuleData::GetRotationForce() const
	{
		return m_RotationForce;
	}

	const uint32_t& JumpModuleData::GetRotationDistribution() const
	{
		return m_RotationDistribution;
	}
}