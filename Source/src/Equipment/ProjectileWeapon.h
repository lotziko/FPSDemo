#pragma once

#include "Blueberry\Core\Base.h"
#include "Blueberry\Core\ObjectPtr.h"
#include "Blueberry\Events\Event.h"
#include "EquipmentItem.h"
#include "Audio\SfxSound.h"

namespace Blueberry
{
	class Animator;

	using AmmoCountUpdateEvent = Event<>;

	class ProjectileWeapon : public EquipmentItem
	{
		OBJECT_DECLARATION(ProjectileWeapon)

	public:
		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		virtual void Equip() override;
		virtual void Unequip() override;

		AmmoCountUpdateEvent& GetAmmoCountUpdated();
		uint32_t GetAmmoCount();
		uint32_t GetMaxAmmoCount();

	private:
		void UpdateFov();
		void ApplyRecoil();

	private:
		ObjectPtr<Transform> m_BarrelPivot;
		SfxSoundData m_ShootSfx;
		SfxSoundData m_DryShootSfx;
		List<DelayedSfxSoundData> m_ReloadSfx;
		List<Vector2> m_RecoilPattern;

		ObjectPtr<Animator> m_WeaponAnimator;
		AmmoCountUpdateEvent m_AmmoCountUpdated;

		bool m_IsFiring = false;
		float m_FireTimer = 0.0f;
		float m_ReloadTimer = 0.0f;
		uint32_t m_BulletCount = 0;
		uint32_t m_BulletsFired = 0;
	};
}