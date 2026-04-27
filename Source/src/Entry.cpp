#include "Entry.h"

#include "Blueberry\Core\ClassDB.h"
#include "Audio\SfxSound.h"
#include "Equipment\EquipmentItemInfo.h"
#include "Equipment\EquipmentItem.h"
#include "Equipment\EquipmentPickup.h"
#include "Equipment\Unarmed.h"
#include "Equipment\ProjectileWeapon.h"
#include "Equipment\PlayerEquipmentController.h"
#include "Player\PlayerMovement.h"
#include "Player\PlayerCamera.h"
#include "Physics\Modules.h"
#include "UI\PlayerUI.h"

using namespace Blueberry;

void Entry()
{
	REGISTER_DATA_CLASS(SfxSoundData);
	REGISTER_DATA_CLASS(DelayedSfxSoundData);
	REGISTER_DATA_CLASS(EquipmentMotionStateData);
	REGISTER_CLASS(EquipmentItemInfo);
	REGISTER_CLASS(EquipmentItem);
	REGISTER_CLASS(EquipmentPickup);
	REGISTER_CLASS(Unarmed);
	REGISTER_CLASS(ProjectileWeapon);
	REGISTER_CLASS(PlayerEquipmentController);
	REGISTER_CLASS(PlayerMovement);
	REGISTER_CLASS(PlayerCamera);
	REGISTER_DATA_CLASS(OffsetModuleData);
	REGISTER_DATA_CLASS(BobModuleData);
	REGISTER_DATA_CLASS(NoiseModuleData);
	REGISTER_DATA_CLASS(SwayModuleData);
	REGISTER_DATA_CLASS(StepForceModuleData);
	REGISTER_CLASS(PlayerUI);
}