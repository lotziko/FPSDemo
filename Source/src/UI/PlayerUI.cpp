#include "PlayerUI.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Scene\Components\Canvas.h"
#include "Equipment\PlayerEquipmentController.h"
#include "Equipment\EquipmentItem.h"
#include "Equipment\ProjectileWeapon.h"

#include <RmlUi\Core.h>

namespace Blueberry
{
	OBJECT_DEFINITION(PlayerUI, Component)
	{
		DEFINE_BASE_FIELDS(PlayerUI, Component)
	}

	void PlayerUI::OnCreate()
	{
		m_Canvas = GetEntity()->GetComponent<Canvas>();
		m_EquipmentController = GetEntity()->GetComponentInChildren<PlayerEquipmentController>();
	}

	void PlayerUI::OnEnable()
	{
		m_EquipmentController->GetPlayerItemEquipped().AddCallback<PlayerUI, &PlayerUI::OnPlayerItemEquip>(this);
		OnPlayerItemEquip();
	}

	void PlayerUI::OnDisable()
	{
		m_EquipmentController->GetPlayerItemEquipped().RemoveCallback<PlayerUI, &PlayerUI::OnPlayerItemEquip>(this);
	}

	void PlayerUI::OnPlayerItemEquip()
	{
		EquipmentItem* item = m_EquipmentController->GetEquipedItem();
		if (m_Weapon.IsValid())
		{
			m_Weapon->GetAmmoCountUpdated().RemoveCallback<PlayerUI, &PlayerUI::OnAmmoCountUpdate>(this);
			m_Weapon = nullptr;
		}
		if (item != nullptr && item->IsClassType(ProjectileWeapon::Type))
		{
			m_Weapon = static_cast<ProjectileWeapon*>(item);
			m_Weapon->GetAmmoCountUpdated().AddCallback<PlayerUI, &PlayerUI::OnAmmoCountUpdate>(this);
			OnAmmoCountUpdate();
			SetTextPanelVisible(true);
		}
		else
		{
			SetTextPanelVisible(false);
		}
	}

	void PlayerUI::OnAmmoCountUpdate()
	{
		if (m_Canvas.IsValid() && m_Weapon.IsValid())
		{
			Rml::Element* root = m_Canvas->GetRoot();
			Rml::ElementText* text = static_cast<Rml::ElementText*>(root->GetElementById("ammo_text")->GetFirstChild());
			text->SetText(Rml::CreateString("%d/%d", m_Weapon->GetAmmoCount(), m_Weapon->GetMaxAmmoCount()));
		}
	}

	void PlayerUI::SetTextPanelVisible(bool visible)
	{
		Rml::Element* root = m_Canvas->GetRoot();
		root->GetElementById("ammo_panel")->SetProperty(Rml::PropertyId::Visibility, Rml::Property(visible ? Rml::Style::Visibility::Visible : Rml::Style::Visibility::Hidden));
	}
}