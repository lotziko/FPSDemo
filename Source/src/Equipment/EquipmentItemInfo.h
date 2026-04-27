#pragma once

#include "Blueberry\Core\Object.h"
#include "Physics\Modules.h"

namespace Blueberry
{
	class EquipmentMotionStateData : public Data
	{
		DATA_DECLARATION(EquipmentMotionStateData)

	public:
		EquipmentMotionStateData() = default;

		const Vector3& GetPositionSpringStiffness();
		const Vector3& GetPositionSpringDamping();
		const Vector3& GetRotationSpringStiffness();
		const Vector3& GetRotationSpringDamping();

		const OffsetModuleData& GetOffset();
		const BobModuleData& GetBob();
		const NoiseModuleData& GetNoise();

	private:
		Vector3 m_PositionSpringStiffness;
		Vector3 m_PositionSpringDamping;

		Vector3 m_RotationSpringStiffness;
		Vector3 m_RotationSpringDamping;

		OffsetModuleData m_Offset;
		BobModuleData m_Bob;
		NoiseModuleData m_Noise;
	};

	class EquipmentItemInfo : public Object
	{
		OBJECT_DECLARATION(EquipmentItemInfo)

	public:
		
		const Vector3& GetBasePositionOffset();
		const Vector3& GetBaseRotationOffset();

		const SwayModuleData& GetSway();

		EquipmentMotionStateData* GetIdleState();
		EquipmentMotionStateData* GetWalkState();
		EquipmentMotionStateData* GetRunState();
		EquipmentMotionStateData* GetAimState();

	private:
		Vector3 m_BasePositionOffset;
		Vector3 m_BaseRotationOffset;

		SwayModuleData m_Sway;

		EquipmentMotionStateData m_IdleState;
		EquipmentMotionStateData m_WalkState;
		EquipmentMotionStateData m_RunState;
		EquipmentMotionStateData m_AimState;
	};
}