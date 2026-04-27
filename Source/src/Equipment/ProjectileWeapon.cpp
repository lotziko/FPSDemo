#include "ProjectileWeapon.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Core\Time.h"
#include "Blueberry\Core\Timer.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Scene\Components\Animator.h"
#include "Blueberry\Scene\Components\Collider.h"
#include "Blueberry\Scene\Components\PhysicsBody.h"
#include "Blueberry\Scene\Components\Camera.h"
#include "Blueberry\Animations\AnimationGraph.h"
#include "Blueberry\Physics\Physics.h"
#include "Blueberry\Input\Input.h"

#include "Player\PlayerMovement.h"
#include "Player\PlayerCamera.h"
#include "Equipment\PlayerEquipmentController.h"

namespace Blueberry
{
	OBJECT_DEFINITION(ProjectileWeapon, EquipmentItem)
	{
		DEFINE_BASE_FIELDS(ProjectileWeapon, EquipmentItem)
		DEFINE_FIELD(ProjectileWeapon, m_BarrelPivot, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
		DEFINE_FIELD(ProjectileWeapon, m_ShootSfx, BindingType::Data, FieldOptions().SetObjectType(&SfxSoundData::Type))
		DEFINE_FIELD(ProjectileWeapon, m_DryShootSfx, BindingType::Data, FieldOptions().SetObjectType(&SfxSoundData::Type))
		DEFINE_FIELD(ProjectileWeapon, m_ReloadSfx, BindingType::DataList, FieldOptions().SetObjectType(&DelayedSfxSoundData::Type))
		DEFINE_FIELD(ProjectileWeapon, m_RecoilPattern, BindingType::Vector2List, FieldOptions())
		DEFINE_ITERATOR(UpdatableComponent)
	}

	static size_t s_FireId = TO_HASH("Fire");
	static size_t s_ReloadId = TO_HASH("Reload");
	static size_t s_HideId = TO_HASH("Hide");

	void ProjectileWeapon::OnCreate()
	{
		EquipmentItem::OnCreate();
		m_WeaponAnimator = GetEntity()->GetComponentInChildren<Animator>();
	}

	void ProjectileWeapon::OnUpdate()
	{
		float time = Time::GetTime();
		if (Input::IsKeyPressed(KeyCode::MouseLeft))
		{
			if (m_BulletCount > 0)
			{
				m_BulletsFired = 0;
				m_IsFiring = true;
			}
			else
			{
				m_DryShootSfx.Play();
			}
		}

		if (Input::IsKeyReleased(KeyCode::MouseLeft))
		{
			m_IsFiring = false;
		}

		if (m_IsFiring)
		{
			if (m_FireTimer < time && m_ReloadTimer < time)
			{
				if (m_BulletCount > 0 && Input::IsKeyDown(KeyCode::MouseLeft))
				{
					--m_BulletCount;
					++m_BulletsFired;
					m_FireTimer = time + 0.1f;

					m_WeaponAnimator->SetTrigger(s_FireId);
					m_ArmsAnimator->SetTrigger(s_FireId);
					Transform* transform = m_BarrelPivot.Get();
					if (transform != nullptr)
					{
						Vector3 position = transform->GetPosition();
						Vector3 direction = Vector3::Transform(Vector3(0, 0, 1), transform->GetRotation());
						RaycastHitData hitData;
						if (Physics::Raycast(position, direction, 100.0f, hitData))
						{
							if (hitData.collider != nullptr)
							{
								PhysicsBody* body = hitData.collider->GetPhysicsBody();
								if (body != nullptr)
								{
									body->AddImpulse(direction * 100.0f, hitData.position);
								}
							}
						}
					}
					ApplyRecoil();
					m_ShootSfx.Play();
					m_AmmoCountUpdated.Invoke();
				}
			}
		}
		else
		{
			if (m_ReloadTimer < time && Input::IsKeyPressed(KeyCode::R))
			{
				m_BulletCount = 30;
				m_ReloadTimer = time + 2.5f;
				m_WeaponAnimator->SetTrigger(s_ReloadId);
				m_ArmsAnimator->SetTrigger(s_ReloadId);
				for (DelayedSfxSoundData& sfx : m_ReloadSfx)
				{
					sfx.Play(this);
				}
				m_AmmoCountUpdated.Invoke();
			}
		}
		UpdateFov();
	}

	void ProjectileWeapon::Equip()
	{
		EquipmentItem::Equip();
	}

	void ProjectileWeapon::Unequip()
	{
		m_WeaponAnimator->SetTrigger(s_HideId);
		m_ArmsAnimator->SetTrigger(s_HideId);
	}

	AmmoCountUpdateEvent& ProjectileWeapon::GetAmmoCountUpdated()
	{
		return m_AmmoCountUpdated;
	}

	uint32_t ProjectileWeapon::GetAmmoCount()
	{
		return m_BulletCount;
	}

	uint32_t ProjectileWeapon::GetMaxAmmoCount()
	{
		return 30;
	}

	void ProjectileWeapon::UpdateFov()
	{
		PlayerCamera* camera = m_EquipmentController->GetCamera();
		PlayerMovement* movement = m_EquipmentController->GetMovement();
		if (movement->IsAiming())
		{
			camera->SetTargetFov(50.0f);
		}
		else
		{
			camera->SetTargetFov(0.0f);
		}
	}

	void ProjectileWeapon::ApplyRecoil()
	{
		PlayerCamera* camera = m_EquipmentController->GetCamera();
		PlayerMovement* movement = m_EquipmentController->GetMovement();
		if (camera != nullptr && movement != nullptr)
		{
			float aimMultiplier = movement->IsAiming() ? 0.9f : 1.0f;

			Vector3 positionForce = Vector3(0, 0, -1);
			Vector3 rotationForce = Vector3(-0.5f, 0, -0.25f);

			positionForce += positionForce * Vector3(Math::GetRandomFloat(-0.3f, 0.3f), Math::GetRandomFloat(-0.3f, 0.3f), Math::GetRandomFloat(-0.2f, 0.2f));
			rotationForce += rotationForce * Vector3(Math::GetRandomFloat(-0.3f, 0.3f), Math::GetRandomFloat(-0.3f, 0.3f), Math::GetRandomFloat(-0.2f, 0.2f));

			camera->AddPositionRecoilForce(positionForce * aimMultiplier / 100.0f, 2);
			camera->AddRotationRecoilForce(rotationForce * aimMultiplier, 2);

			float recoilMultiplier = 3.0f;
			Vector2 recoilOffset = m_RecoilPattern[m_BulletsFired % m_RecoilPattern.size()];
			camera->MoveCamera(recoilOffset * (recoilMultiplier * aimMultiplier * Time::GetDeltaTime()));
		}
	}
}