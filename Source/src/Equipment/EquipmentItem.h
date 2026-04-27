#pragma once

#include "Blueberry\Scene\Components\Component.h"
#include "Blueberry\Core\ObjectPtr.h"

namespace Blueberry
{
	class EquipmentItemInfo;
	class PlayerEquipmentController;
	class Animator;
	class AnimationGraph;
	class Transform;

	class EquipmentItem : public Component
	{
		OBJECT_DECLARATION(EquipmentItem)

	public:
		virtual void OnCreate();

		virtual void Equip();
		virtual void Unequip();

		EquipmentItemInfo* GetItemInfo() const;
		AnimationGraph* GetArmsAnimationGraph() const;
		Transform* GetPivot() const;

	private:
		ObjectPtr<EquipmentItemInfo> m_ItemInfo;
		ObjectPtr<AnimationGraph> m_ArmsAnimationGraph;
		ObjectPtr<Transform> m_Pivot;

	protected:
		ObjectPtr<PlayerEquipmentController> m_EquipmentController;
		ObjectPtr<Animator> m_ArmsAnimator;
	};
}