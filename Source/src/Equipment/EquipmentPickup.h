#pragma once

#include "Blueberry\Scene\Components\Component.h"
#include "Blueberry\Core\ObjectPtr.h"

namespace Blueberry
{
	class EquipmentItemInfo;

	class EquipmentPickup : public Component
	{
		OBJECT_DECLARATION(EquipmentPickup)

	public:
		EquipmentItemInfo* GetItemInfo() const;

	private:
		ObjectPtr<EquipmentItemInfo> m_ItemInfo;
	};
}