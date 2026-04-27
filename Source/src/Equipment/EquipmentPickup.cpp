#include "EquipmentPickup.h"

#include "Blueberry\Core\ClassDB.h"
#include "Equipment\EquipmentItemInfo.h"

namespace Blueberry
{
	OBJECT_DEFINITION(EquipmentPickup, Component)
	{
		DEFINE_BASE_FIELDS(EquipmentPickup, Component)
		DEFINE_FIELD(EquipmentPickup, m_ItemInfo, BindingType::ObjectPtr, FieldOptions().SetObjectType(&EquipmentItemInfo::Type))
	}

	EquipmentItemInfo* EquipmentPickup::GetItemInfo() const
	{
		return m_ItemInfo.Get();
	}
}