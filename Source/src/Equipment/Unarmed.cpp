#include "Unarmed.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Scene\Components\Animator.h"
#include "Blueberry\Input\Input.h"

#include "Player\PlayerMovement.h"
#include "Equipment\PlayerEquipmentController.h"

namespace Blueberry
{
	OBJECT_DEFINITION(Unarmed, EquipmentItem)
	{
		DEFINE_BASE_FIELDS(Unarmed, EquipmentItem)
		DEFINE_ITERATOR(UpdatableComponent)
	}

	static size_t s_RunningId = TO_HASH("Running");
	static size_t s_ArmsAreVisibleId = TO_HASH("Arms Are Visible");
	static size_t s_HideId = TO_HASH("Hide");

	void Unarmed::OnCreate()
	{
		EquipmentItem::OnCreate();
		m_Movement = GetEntity()->GetComponentInParent<PlayerMovement>();
	}

	void Unarmed::OnEnable()
	{
		m_Movement->GetPlayerStartedRunning().AddCallback<Unarmed, &Unarmed::OnPlayerStartRunning>(this);
		m_Movement->GetPlayerEndedRunning().AddCallback<Unarmed, &Unarmed::OnPlayerEndRunning>(this);
	}

	void Unarmed::OnDisable()
	{
		m_Movement->GetPlayerStartedRunning().RemoveCallback<Unarmed, &Unarmed::OnPlayerStartRunning>(this);
		m_Movement->GetPlayerEndedRunning().RemoveCallback<Unarmed, &Unarmed::OnPlayerEndRunning>(this);
	}

	void Unarmed::OnUpdate()
	{
		if (!m_ArmsAreVisible && Input::IsKeyPressed(KeyCode::MouseLeft))
		{
			m_ArmsAreVisible = true;
			m_ArmsAnimator->SetBool(s_ArmsAreVisibleId, true);
		}
	}

	void Unarmed::Equip()
	{
		EquipmentItem::Equip();
		m_ArmsAreVisible = false;
	}

	void Unarmed::Unequip()
	{
		if (m_ArmsAreVisible)
		{
			m_ArmsAnimator->SetTrigger(s_HideId);
		}
	}

	void Unarmed::OnPlayerStartRunning()
	{
		m_ArmsAreVisible = false;
		m_ArmsAnimator->SetBool(s_ArmsAreVisibleId, false);
		m_ArmsAnimator->SetBool(s_RunningId, true);
	}

	void Unarmed::OnPlayerEndRunning()
	{
		m_ArmsAnimator->SetBool(s_RunningId, false);
	}
}