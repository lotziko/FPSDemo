#pragma once

#include "Blueberry\Scene\Components\Component.h"
#include "Blueberry\Core\ObjectPtr.h"
#include "Blueberry\Events\Event.h"
#include "Physics\Spring.h"

namespace Blueberry
{
	class PlayerMovement;
	class PlayerCamera;
	class Transform;
	class Animator;
	class Unarmed;
	class EquipmentItem;
	class EquipmentItemInfo;
	class EquipmentMotionStateData;

	using PlayerItemEquipEvent = Event<>;

	class PlayerEquipmentController : public Component
	{
		OBJECT_DECLARATION(PlayerEquipmentController)

	public:
		virtual void OnCreate() final;
		virtual void OnEnable() final;
		virtual void OnDisable() final;
		virtual void OnFixedUpdate() final;
		virtual void OnUpdate() final;

		Animator* GetArmsAnimator();
		PlayerCamera* GetCamera();
		PlayerMovement* GetMovement();

		PlayerItemEquipEvent& GetPlayerItemEquipped();
		EquipmentItem* GetEquipedItem() const;

	private:
		void EquipItem(size_t index);

		void UpdatePhysics();
		void UpdateState();
		void UpdateOffset();
		void UpdateBob();
		void UpdateNoise();
		void UpdateSway();
		void ApplyStepForce();

		void UpdateTransform();

		void OnPlayerStep();

	private:
		ObjectPtr<Transform> m_Pivot;
		ObjectPtr<Animator> m_ArmsAnimator;

		ObjectPtr<PlayerMovement> m_Movement;
		ObjectPtr<PlayerCamera> m_Camera;
		ObjectPtr<Transform> m_Transform;
		List<ObjectPtr<EquipmentItem>> m_Items;
		size_t m_CurrentItemIndex = UINT64_MAX;
		size_t m_RequestedItemIndex = UINT64_MAX;
		float m_EquipTimer = 0.0f;
		ObjectPtr<EquipmentItemInfo> m_ItemInfo;
		EquipmentMotionStateData* m_CurrentState = nullptr;

		List<ObjectPtr<Transform>> m_PivotChildren;
		Vector3 m_ModelOffset;

		Spring m_PositionSpring;
		Spring m_RotationSpring;

		Vector3 m_StatePosition;
		Vector3 m_StateRotation;
		float m_CurrentBobParam;
		bool m_LastFootDown;

		PlayerItemEquipEvent m_PlayerItemEquipped;
	};
}