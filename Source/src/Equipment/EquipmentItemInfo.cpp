#include "EquipmentItemInfo.h"

#include "Blueberry\Core\ClassDB.h"

namespace Blueberry
{
	DATA_DEFINITION(EquipmentMotionStateData)
	{
		DEFINE_FIELD(EquipmentMotionStateData, m_PositionSpringStiffness, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentMotionStateData, m_PositionSpringDamping, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentMotionStateData, m_RotationSpringStiffness, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentMotionStateData, m_RotationSpringDamping, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentMotionStateData, m_Offset, BindingType::Data, FieldOptions().SetObjectType(&OffsetModuleData::Type))
		DEFINE_FIELD(EquipmentMotionStateData, m_Bob, BindingType::Data, FieldOptions().SetObjectType(&BobModuleData::Type))
		DEFINE_FIELD(EquipmentMotionStateData, m_Noise, BindingType::Data, FieldOptions().SetObjectType(&NoiseModuleData::Type))
	}

	OBJECT_DEFINITION(EquipmentItemInfo, Object)
	{
		DEFINE_BASE_FIELDS(EquipmentItemInfo, Object)
		DEFINE_FIELD(EquipmentItemInfo, m_BasePositionOffset, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentItemInfo, m_BaseRotationOffset, BindingType::Vector3, {})
		DEFINE_FIELD(EquipmentItemInfo, m_Sway, BindingType::Data, FieldOptions().SetObjectType(&SwayModuleData::Type))
		DEFINE_FIELD(EquipmentItemInfo, m_IdleState, BindingType::Data, FieldOptions().SetObjectType(&EquipmentMotionStateData::Type))
		DEFINE_FIELD(EquipmentItemInfo, m_WalkState, BindingType::Data, FieldOptions().SetObjectType(&EquipmentMotionStateData::Type))
		DEFINE_FIELD(EquipmentItemInfo, m_RunState, BindingType::Data, FieldOptions().SetObjectType(&EquipmentMotionStateData::Type))
		DEFINE_FIELD(EquipmentItemInfo, m_AimState, BindingType::Data, FieldOptions().SetObjectType(&EquipmentMotionStateData::Type))
	}

	const Vector3& EquipmentMotionStateData::GetPositionSpringStiffness()
	{
		return m_PositionSpringStiffness;
	}

	const Vector3& EquipmentMotionStateData::GetPositionSpringDamping()
	{
		return m_PositionSpringDamping;
	}

	const Vector3& EquipmentMotionStateData::GetRotationSpringStiffness()
	{
		return m_RotationSpringStiffness;
	}

	const Vector3& EquipmentMotionStateData::GetRotationSpringDamping()
	{
		return m_RotationSpringDamping;
	}

	const OffsetModuleData& EquipmentMotionStateData::GetOffset()
	{
		return m_Offset;
	}

	const BobModuleData& EquipmentMotionStateData::GetBob()
	{
		return m_Bob;
	}

	const NoiseModuleData& EquipmentMotionStateData::GetNoise()
	{
		return m_Noise;
	}

	const Vector3& EquipmentItemInfo::GetBasePositionOffset()
	{
		return m_BasePositionOffset;
	}

	const Vector3& EquipmentItemInfo::GetBaseRotationOffset()
	{
		return m_BaseRotationOffset;
	}

	const SwayModuleData& EquipmentItemInfo::GetSway()
	{
		return m_Sway;
	}

	EquipmentMotionStateData* EquipmentItemInfo::GetIdleState()
	{
		return &m_IdleState;
	}

	EquipmentMotionStateData* EquipmentItemInfo::GetWalkState()
	{
		return &m_WalkState;
	}

	EquipmentMotionStateData* EquipmentItemInfo::GetRunState()
	{
		return &m_RunState;
	}

	EquipmentMotionStateData* EquipmentItemInfo::GetAimState()
	{
		return &m_AimState;
	}
}