#pragma once

#include "Blueberry\Scene\Components\Component.h"

namespace Blueberry
{
	class Canvas;
	class PlayerEquipmentController;
	class EquipmentItem;
	class ProjectileWeapon;

	class PlayerUI : public Component
	{
		OBJECT_DECLARATION(PlayerUI)

	public:
		virtual void OnCreate() final;
		virtual void OnEnable() final;
		virtual void OnDisable() final;

	private:
		void OnPlayerItemEquip();
		void OnAmmoCountUpdate();
		void SetTextPanelVisible(bool visible);

	private:
		ObjectPtr<Canvas> m_Canvas;
		ObjectPtr<PlayerEquipmentController> m_EquipmentController;
		ObjectPtr<ProjectileWeapon> m_Weapon;
	};
}