#include "EquipmentItem.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Scene\Components\Animator.h"
#include "Blueberry\Animations\AnimationGraph.h"
#include "EquipmentItemInfo.h"
#include "PlayerEquipmentController.h"

namespace Blueberry
{
	OBJECT_DEFINITION(EquipmentItem, Component)
	{
		DEFINE_BASE_FIELDS(EquipmentItem, Component)
		DEFINE_FIELD(EquipmentItem, m_ItemInfo, BindingType::ObjectPtr, FieldOptions().SetObjectType(&EquipmentItemInfo::Type))
		DEFINE_FIELD(EquipmentItem, m_ArmsAnimationGraph, BindingType::ObjectPtr, FieldOptions().SetObjectType(&AnimationGraph::Type))
		DEFINE_FIELD(EquipmentItem, m_Pivot, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
	}

	void EquipmentItem::OnCreate()
	{
		m_EquipmentController = GetEntity()->GetComponentInParent<PlayerEquipmentController>();
		m_ArmsAnimator = m_EquipmentController->GetArmsAnimator();
	}

	void EquipmentItem::Equip()
	{
		if (m_EquipmentController == nullptr)
		{
			OnCreate();
		}
		m_EquipmentController->GetArmsAnimator()->SetAnimationGraph(m_ArmsAnimationGraph.Get());
	}

	void EquipmentItem::Unequip()
	{
	}

	EquipmentItemInfo* EquipmentItem::GetItemInfo() const
	{
		return m_ItemInfo.Get();
	}

	AnimationGraph* EquipmentItem::GetArmsAnimationGraph() const
	{
		return m_ArmsAnimationGraph.Get();
	}

	Transform* EquipmentItem::GetPivot() const
	{
		return m_Pivot.Get();
	}
}