#pragma once

#include "Blueberry\Core\Base.h"
#include "Blueberry\Core\ObjectPtr.h"
#include "EquipmentItem.h"

namespace Blueberry
{
	class PlayerMovement;
	class PlayerEquipmentController;
	class Animator;

	class Unarmed : public EquipmentItem
	{
		OBJECT_DECLARATION(Unarmed)

	public:
		virtual void OnCreate() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnUpdate() override;

		virtual void Equip() override;
		virtual void Unequip() override;

	private:
		void OnPlayerStartRunning();
		void OnPlayerEndRunning();

	private:
		ObjectPtr<PlayerMovement> m_Movement;
		bool m_ArmsAreVisible = false;
	};
}