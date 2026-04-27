#include "PlayerEquipmentController.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Core\Time.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Scene\Components\Animator.h"
#include "Blueberry\Scene\Components\Collider.h"
#include "Blueberry\Input\Input.h"
#include "Blueberry\Physics\Physics.h"
#include "Player\PlayerMovement.h"
#include "Player\PlayerCamera.h"
#include "EquipmentItemInfo.h"
#include "EquipmentItem.h"
#include "EquipmentPickup.h"
#include "Unarmed.h"

namespace Blueberry
{
	OBJECT_DEFINITION(PlayerEquipmentController, Component)
	{
		DEFINE_BASE_FIELDS(PlayerEquipmentController, Component)
		DEFINE_FIELD(PlayerEquipmentController, m_Pivot, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
		DEFINE_FIELD(PlayerEquipmentController, m_ArmsAnimator, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Animator::Type))
		DEFINE_ITERATOR(UpdatableComponent)
	}

	void PlayerEquipmentController::OnCreate()
	{
		m_PositionSpring = Spring(Vector3(0, 0, 0), 25.0f);
		m_RotationSpring = Spring(Vector3(0, 0, 0), 25.0f);
		m_Movement = GetEntity()->GetComponentInParent<PlayerMovement>();
		m_Camera = GetEntity()->GetComponentInParent<PlayerCamera>();
		m_Transform = GetTransform();
		for (EquipmentItem* item : GetEntity()->GetComponentsInChildren<EquipmentItem>())
		{
			m_Items.push_back(item);
		}

		for (auto& child : m_Pivot->GetChildren())
		{
			m_PivotChildren.push_back(child.Get());
		}
		m_ModelOffset = m_PivotChildren[0]->GetLocalPosition();
	}

	void PlayerEquipmentController::OnEnable()
	{
		EquipItem(0);
		m_Movement->GetPlayerStepped().AddCallback<PlayerEquipmentController, &PlayerEquipmentController::OnPlayerStep>(this);
	}

	void PlayerEquipmentController::OnDisable()
	{
		m_Movement->GetPlayerStepped().RemoveCallback<PlayerEquipmentController, &PlayerEquipmentController::OnPlayerStep>(this);
	}

	void PlayerEquipmentController::OnFixedUpdate()
	{
		UpdatePhysics();
	}

	void PlayerEquipmentController::OnUpdate()
	{
		UpdateTransform();

		float time = Time::GetTime();
		if (Input::IsKeyPressed(KeyCode::E) && m_RequestedItemIndex == UINT64_MAX)
		{
			Transform* cameraTransform = m_Camera->GetTransform();
			RaycastHitData hitData;
			if (Physics::Raycast(cameraTransform->GetPosition(), Vector3::Transform(Vector3(0, 0, 1), cameraTransform->GetRotation()), 1.0f, hitData))
			{
				Entity* entity = hitData.collider->GetEntity();
				EquipmentPickup* pickup = entity->GetComponent<EquipmentPickup>();
				if (pickup != nullptr)
				{
					EquipmentItemInfo* info = pickup->GetItemInfo();
					for (size_t i = 0; i < m_Items.size(); ++i)
					{
						if (m_Items[i]->GetItemInfo() == info)
						{
							m_RequestedItemIndex = i;
							break;
						}
					}
				}
			}
			else
			{
				m_RequestedItemIndex = 0;
			}
			if (m_CurrentItemIndex != UINT64_MAX)
			{
				EquipmentItem* previousItem = m_Items[m_CurrentItemIndex].Get();
				previousItem->Unequip();
			}
			m_EquipTimer = time + 1.0f;
		}

		if (m_EquipTimer < time && m_RequestedItemIndex != UINT64_MAX)
		{
			EquipItem(m_RequestedItemIndex);
			m_RequestedItemIndex = UINT64_MAX;
		}
	}

	Animator* PlayerEquipmentController::GetArmsAnimator()
	{
		return m_ArmsAnimator.Get();
	}

	PlayerCamera* PlayerEquipmentController::GetCamera()
	{
		return m_Camera.Get();
	}

	PlayerMovement* PlayerEquipmentController::GetMovement()
	{
		return m_Movement.Get();
	}

	PlayerItemEquipEvent& PlayerEquipmentController::GetPlayerItemEquipped()
	{
		return m_PlayerItemEquipped;
	}

	EquipmentItem* PlayerEquipmentController::GetEquipedItem() const
	{
		if (m_CurrentItemIndex == UINT64_MAX)
		{
			return nullptr;
		}
		return m_Items[m_CurrentItemIndex].Get();
	}

	void PlayerEquipmentController::EquipItem(size_t index)
	{
		if (m_CurrentItemIndex != UINT64_MAX)
		{
			EquipmentItem* previousItem = m_Items[m_CurrentItemIndex].Get();
			previousItem->GetEntity()->SetActive(false);
		}
		EquipmentItem* item = m_Items[index].Get();
		if (item != nullptr)
		{
			m_CurrentItemIndex = index;
			item->GetEntity()->SetActive(true);
			item->Equip();
			m_ItemInfo = item->GetItemInfo();
			m_ArmsAnimator->SetAnimationGraph(item->GetArmsAnimationGraph());

			m_Pivot->SetLocalPosition({});
			m_Pivot->SetLocalRotation({});

			for (auto& child : m_PivotChildren)
			{
				child->SetParent(m_Transform->GetParent());
				child->SetLocalPosition(m_ModelOffset);
				child->SetLocalRotation({});
			}

			m_Pivot->SetParent(m_Transform->GetParent());
			m_Pivot->SetPosition(item->GetPivot()->GetPosition());
			m_Pivot->SetLocalRotation({});

			m_Transform->SetPosition(m_Pivot->GetPosition());
			m_Transform->SetRotation(m_Pivot->GetRotation());

			m_Pivot->SetParent(m_Transform.Get());
			for (auto& child : m_PivotChildren)
			{
				child->SetParent(m_Pivot.Get());
			}

			m_PivotChildren[0]->SetLocalEulerRotationHint(Vector3(item->IsClassType(Unarmed::Type) ? 0 : 90, 0, 0));

			m_Transform->SetLocalPosition(m_Transform->GetLocalPosition() + m_ItemInfo->GetBasePositionOffset());
			m_Transform->SetLocalEulerRotation(m_Transform->GetLocalEulerRotation() + Math::ToRadians(m_ItemInfo->GetBaseRotationOffset()));
		}
		m_PlayerItemEquipped.Invoke();
	}

	void PlayerEquipmentController::UpdatePhysics()
	{
		if (m_ItemInfo.IsValid())
		{
			float springForceMultiplier = 1.0f;

			m_StatePosition = {};
			m_StateRotation = {};

			UpdateState();
			UpdateOffset();
			UpdateBob();
			UpdateNoise();
			UpdateSway();

			m_StatePosition *= springForceMultiplier;
			m_StateRotation *= springForceMultiplier;

			m_PositionSpring.AddForce(m_StatePosition);
			m_RotationSpring.AddForce(m_StateRotation);

			m_PositionSpring.FixedUpdate();
			m_RotationSpring.FixedUpdate();
		}
	}

	void PlayerEquipmentController::UpdateState()
	{
		if (m_Movement->IsRunning() && m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			m_CurrentState = m_ItemInfo->GetRunState();
		}
		else if (m_Movement->IsAiming())
		{
			m_CurrentState = m_ItemInfo->GetAimState();
		}
		else if (m_Movement->IsWalking() && m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			m_CurrentState = m_ItemInfo->GetWalkState();
		}
		else
		{
			m_CurrentState = m_ItemInfo->GetIdleState();
		}

		m_PositionSpring.Adjust(m_CurrentState->GetPositionSpringStiffness(), m_CurrentState->GetPositionSpringDamping());
		m_RotationSpring.Adjust(m_CurrentState->GetRotationSpringStiffness(), m_CurrentState->GetRotationSpringDamping());
	}

	void PlayerEquipmentController::UpdateOffset()
	{
		const OffsetModuleData& offset = m_CurrentState->GetOffset();
		if (!offset.IsEnabled())
		{
			return;
		}

		m_StatePosition += offset.GetPositionOffset() * 0.0001f;
		m_StateRotation += offset.GetRotationOffset() * 0.02f;
	}

	void PlayerEquipmentController::UpdateBob()
	{
		const BobModuleData& bob = m_CurrentState->GetBob();
		if (!bob.IsEnabled() || (m_Movement->GetVelocity().LengthSquared() < 0.1f))
		{
			return;
		}

		m_CurrentBobParam = m_Movement->GetMoveCycle() * Math::Pi;

		if (m_LastFootDown)
		{
			m_CurrentBobParam += Math::Pi;
		}

		float positionBobOffset = 0.0f;
		Vector3 posBobAmplitude = bob.GetPositionAmplitude() * 0.00001f;
		posBobAmplitude.x *= -1;

		m_StatePosition.x += std::cos(m_CurrentBobParam + positionBobOffset) * posBobAmplitude.x;
		m_StatePosition.y += std::cos(m_CurrentBobParam * 2 + positionBobOffset) * posBobAmplitude.y;
		m_StatePosition.z += std::cos(m_CurrentBobParam + positionBobOffset) * posBobAmplitude.z;

		float rotationBobOffset = 0.5f;
		Vector3 rotBobAmplitude = bob.GetRotationAmplitude() * 0.001f;

		m_StateRotation.x += std::cos(m_CurrentBobParam * 2 + rotationBobOffset) * rotBobAmplitude.x;
		m_StateRotation.y += std::cos(m_CurrentBobParam + rotationBobOffset) * rotBobAmplitude.y;
		m_StateRotation.z += std::cos(m_CurrentBobParam + rotationBobOffset) * rotBobAmplitude.z;
	}

	void PlayerEquipmentController::UpdateNoise()
	{
		const NoiseModuleData& noise = m_CurrentState->GetNoise();
		if (noise.IsEnabled())
		{
			float jitter = Math::GetRandomFloat(0.0f, noise.GetMaxJitter());
			float timeScale = Time::GetTime() * noise.GetNoiseSpeed();
			Vector3 posNoiseAmplitude = noise.GetPositionAmplitude();
			Vector3 rotNoiseAmplitude = noise.GetRotationAmplitude();

			m_StatePosition.x += (Math::PerlinNoise(jitter, timeScale) - 0.5f) * posNoiseAmplitude.x / 1000;
			m_StatePosition.y += (Math::PerlinNoise(jitter + 1.0f, timeScale) - 0.5f) * posNoiseAmplitude.y / 1000;
			m_StatePosition.z += (Math::PerlinNoise(jitter + 2.0f, timeScale) - 0.5f) * posNoiseAmplitude.z / 1000;
			
			m_StateRotation.x += (Math::PerlinNoise(jitter, timeScale) - 0.5f) * rotNoiseAmplitude.x / 10;
			m_StateRotation.y += (Math::PerlinNoise(jitter + 1.0f, timeScale) - 0.5f) * rotNoiseAmplitude.y / 10;
			m_StateRotation.z += (Math::PerlinNoise(jitter + 2.0f, timeScale) - 0.5f) * rotNoiseAmplitude.z / 10;
		}
	}

	void PlayerEquipmentController::UpdateSway()
	{
		const SwayModuleData& sway = m_ItemInfo->GetSway();
		float multiplier = (m_Movement->IsAiming() ? m_Camera->GetSway().GetAimMultiplier() : 1.0f) * Time::GetFixedDeltaTime();

		Vector2 lookInput = m_Camera->GetLookAxis();
		lookInput *= sway.GetLookInputMultiplier();
		lookInput = Math::ClampMagnitude(lookInput, sway.GetMaxLookInput());

		Vector3 swayVelocity = m_Movement->GetVelocity();

		if (std::abs(swayVelocity.y) < 1.5f)
		{
			swayVelocity.y = 0.0f;
		}

		Vector3 localVelocity = Math::MultiplyVector(m_Pivot->GetWorldToLocalMatrix(), swayVelocity / 60.0f);

		Vector3 lookPositionSway = sway.GetLookPositionSway();
		Vector3 lookRotationSway = sway.GetLookRotationSway();
		Vector3 strafePositionSway = sway.GetStrafePositionSway();
		Vector3 strafeRotationSway = sway.GetStrafeRotationSway();

		m_PositionSpring.AddForce(Vector3(
			lookInput.x * lookPositionSway.x * 0.125f,
			lookInput.y * lookPositionSway.y * -0.125f,
			lookInput.y * lookPositionSway.z * -0.125f) * multiplier);

		m_RotationSpring.AddForce(Vector3(
			lookInput.y * lookRotationSway.x * 1.25f,
			lookInput.x * lookRotationSway.y * -1.25f,
			lookInput.x * lookRotationSway.z * -1.25f) * multiplier);

		m_PositionSpring.AddForce(Vector3(
			localVelocity.x * strafePositionSway.x * 0.08f,
			-std::abs(localVelocity.x * strafePositionSway.y * 0.08f),
			-localVelocity.z * strafePositionSway.z * 0.08f) * multiplier);

		m_RotationSpring.AddForce(Vector3(
			-std::abs(localVelocity.x * strafeRotationSway.x * 8.0f),
			-localVelocity.x * strafeRotationSway.y * 8.0f,
			localVelocity.x * strafeRotationSway.z * 8.0f) * multiplier);
	}

	void PlayerEquipmentController::ApplyStepForce()
	{
		Vector3 stepPosForce = Vector3(0, -5, 0);
		Vector3 stepRotForce = Vector3(6, 0, -3);

		m_PositionSpring.AddForce(stepPosForce * 0.0001f);
		m_RotationSpring.AddForce(stepRotForce * 0.01f);
	}

	void PlayerEquipmentController::UpdateTransform()
	{
		m_PositionSpring.Update();
		m_RotationSpring.Update();

		Vector3 localPosition = m_PositionSpring.GetPosition();
		Vector3 localRotation = m_RotationSpring.GetPosition();

		m_Pivot->SetLocalPosition(localPosition);
		m_Pivot->SetLocalEulerRotation(Vector3(Math::ToRadians(localRotation.x), Math::ToRadians(localRotation.y), Math::ToRadians(localRotation.z)));
	}

	void PlayerEquipmentController::OnPlayerStep()
	{
		if (m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			ApplyStepForce();
		}

		m_LastFootDown = !m_LastFootDown;
	}
}