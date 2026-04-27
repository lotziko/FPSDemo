#include "PlayerCamera.h"

#include "PlayerMovement.h"
#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Scene\Components\Camera.h"
#include "Blueberry\Scene\Entity.h"
#include "Blueberry\Core\Time.h"
#include "Blueberry\Input\Input.h"
#include "Blueberry\Input\Cursor.h"

namespace Blueberry
{
	OBJECT_DEFINITION(PlayerCamera, Component)
	{
		DEFINE_BASE_FIELDS(PlayerCamera, Component)
		DEFINE_FIELD(PlayerCamera, m_PlayerRootTransform, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
		DEFINE_FIELD(PlayerCamera, m_ViewRootTransform, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
		DEFINE_FIELD(PlayerCamera, m_CameraTransform, BindingType::ObjectPtr, FieldOptions().SetObjectType(&Transform::Type))
		DEFINE_ITERATOR(UpdatableComponent)
	}
	
	CameraMotionStateData::CameraMotionStateData(const BobModuleData& bob, const NoiseModuleData& noise) : m_Bob(bob), m_Noise(noise)
	{
	}

	const BobModuleData& CameraMotionStateData::GetBob()
	{
		return m_Bob;
	}

	const NoiseModuleData& CameraMotionStateData::GetNoise()
	{
		return m_Noise;
	}

	void PlayerCamera::OnCreate()
	{
		m_PositionSpring = Spring(Vector3(0.02f, 0.02f, 0.02f), Vector3(0.25f, 0.25f, 0.25f), 25.0f);
		m_RotationSpring = Spring(Vector3(0.02f, 0.02f, 0.02f), Vector3(0.25f, 0.25f, 0.25f), 25.0f);
		m_PositionRecoilSpring = Spring(Vector3(0.1f, 0.1f, 0.1f), Vector3(0.4f, 0.3f, 0.3f), 25.0f);
		m_RotationRecoilSpring = Spring(Vector3(0.1f, 0.1f, 0.1f), Vector3(0.4f, 0.3f, 0.3f), 25.0f);
		m_Movement = GetEntity()->GetComponentInParent<PlayerMovement>();
		m_Camera = m_CameraTransform->GetEntity()->GetComponent<Camera>();
		m_Sway = SwayModuleData(1.0f, 5.0f, 0.2f, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.05f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.5f));
		m_Jump = JumpModuleData(true, Vector3(0.0f, 0.0f, 0.0f), 9, Vector3(1.5f, 0.0f, 0.0f), 12);
		m_IdleState = CameraMotionStateData(BobModuleData(true, 1.0f, Vector3(0.35f, 0.5f, 0.0f), Vector3(0.35f, 0.5f, 0.0f)), NoiseModuleData(false, 0.0f, {}, {}));
		m_WalkState = CameraMotionStateData(BobModuleData(true, 1.0f, Vector3(0.0f, -2.0f, 0.0f), Vector3(4.0f, 1.0f, -4.0f)), NoiseModuleData(true, 0.5f, Vector3(0.15f, 0.15f, 0.15f), Vector3(0.2f, 0.2f, 0.2f)));
		m_RunState = CameraMotionStateData(BobModuleData(true, 1.0f, Vector3(1.0f, -5.0f, 0.0f), Vector3(20.0f, 3.0f, 12.0f)), NoiseModuleData(true, 0.75f, Vector3(0.15f, 0.15f, 0.15f), Vector3(0.2f, 0.2f, 0.2f)));
		m_AimState = CameraMotionStateData(BobModuleData(true, 1.5f, Vector3(0.0f, 0.0f, 0.0f), Vector3(-6.0f, 14.0f, 0.5f)), NoiseModuleData(true, 1.0f, Vector3(0.15f, 0.15f, 0.15f), Vector3(0.1f, 0.1f, 0.1f)));
	}

	void PlayerCamera::OnEnable()
	{
		Cursor::SetLocked(true);
		Cursor::SetHidden(true);
		m_Movement->GetPlayerStepped().AddCallback<PlayerCamera, &PlayerCamera::OnPlayerStep>(this);
		m_Movement->GetPlayerJumped().AddCallback<PlayerCamera, &PlayerCamera::OnPlayerJump>(this);
	}

	void PlayerCamera::OnDisable()
	{
		Cursor::SetLocked(false);
		Cursor::SetHidden(false);
		m_Movement->GetPlayerStepped().RemoveCallback<PlayerCamera, &PlayerCamera::OnPlayerStep>(this);
		m_Movement->GetPlayerJumped().RemoveCallback<PlayerCamera, &PlayerCamera::OnPlayerJump>(this);
	}

	void PlayerCamera::OnFixedUpdate()
	{
		UpdateCameraPhysics();
	}

	void PlayerCamera::OnUpdate()
	{
		UpdateCameraTransform();
	}

	const Vector2& PlayerCamera::GetLookAxis() const
	{
		return m_LookAxis;
	}

	const SwayModuleData& PlayerCamera::GetSway() const
	{
		return m_Sway;
	}

	void PlayerCamera::AddPositionRecoilForce(const Vector3& positionForce, int distribution)
	{
		if (distribution > 1)
		{
			m_PositionRecoilSpring.AddDistributedForce(positionForce, distribution);
		}
		else
		{
			m_PositionRecoilSpring.AddForce(positionForce);
		}
	}

	void PlayerCamera::AddRotationRecoilForce(const Vector3& rotationForce, int distribution)
	{
		if (distribution > 1)
		{
			m_RotationRecoilSpring.AddDistributedForce(rotationForce, distribution);
		}
		else
		{
			m_RotationRecoilSpring.AddForce(rotationForce);
		}
	}

	void PlayerCamera::MoveCamera(const Vector2& movement)
	{
		m_Rotation += movement;
	}

	void PlayerCamera::SetTargetFov(float targetFov)
	{
		m_TargetFov = targetFov;
	}

	void PlayerCamera::UpdateCameraPhysics()
	{
		float deltaTime = Time::GetFixedDeltaTime();
		float springForceMultiplier = 0.5f;

		m_StatePosition = {};
		m_StateRotation = {};

		UpdateState();
		UpdateFov();

		UpdateStationaryBob(deltaTime);
		UpdateMovementBob(deltaTime);
		UpdateNoise();
		UpdateSway();

		m_StatePosition *= springForceMultiplier;
		m_StateRotation *= springForceMultiplier;

		m_PositionSpring.AddForce(m_StatePosition);
		m_RotationSpring.AddForce(m_StateRotation);

		m_PositionSpring.FixedUpdate();
		m_RotationSpring.FixedUpdate();
		m_PositionRecoilSpring.FixedUpdate();
		m_RotationRecoilSpring.FixedUpdate();
	}

	void PlayerCamera::UpdateState()
	{
		if (m_Movement->IsRunning() && m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			m_CurrentState = &m_RunState;
		}
		else if (m_Movement->IsWalking() && m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			m_CurrentState = &m_WalkState;
		}
		else
		{
			m_CurrentState = &m_IdleState;
		}
	}

	void PlayerCamera::UpdateFov()
	{
		float targetFov = m_TargetFov > 0.0f ? m_TargetFov : 70.0f;
		float currentFov = m_Camera->GetFieldOfView();
		if (std::abs(targetFov - currentFov) > Math::Epsilon)
		{
			currentFov = Math::MoveTowards(currentFov, targetFov, 90.0f * Time::GetDeltaTime());
			m_Camera->SetFieldOfView(currentFov);
		}
	}

	void PlayerCamera::UpdateStationaryBob(float deltaTime)
	{
		if (!m_Movement->IsAiming())
		{
			return;
		}

		m_CurrentStaticBobParam += deltaTime * m_AimState.GetBob().GetBobSpeed();

		if (m_CurrentStaticBobParam >= Math::Pi * 2.0f)
		{
			m_CurrentStaticBobParam -= Math::Pi * 2.0f;
		}

		UpdateBob(m_CurrentStaticBobParam, &m_AimState, 1.0f);
	}

	void PlayerCamera::UpdateMovementBob(float deltaTime)
	{
		m_CurrentMovingBobParam = m_Movement->GetMoveCycle() * Math::Pi;

		if (m_LastFootDown)
		{
			m_CurrentMovingBobParam += Math::Pi;
		}

		UpdateBob(m_CurrentMovingBobParam, m_CurrentState);
	}

	void PlayerCamera::UpdateBob(float currentBobParam, CameraMotionStateData* state, float mod)
	{
		const BobModuleData& bob = m_CurrentState->GetBob();
		if (bob.IsEnabled())
		{
			float positionBobOffset = 0.0f;
			Vector3 posBobAmplitude = bob.GetPositionAmplitude() * 0.0001f;
			posBobAmplitude.x *= -1;

			m_StatePosition.x += std::cos(currentBobParam + positionBobOffset) * posBobAmplitude.x * mod;
			m_StatePosition.y += std::cos(currentBobParam * 2 + positionBobOffset) * posBobAmplitude.y * mod;
			m_StatePosition.z += std::cos(currentBobParam + positionBobOffset) * posBobAmplitude.z * mod;

			float rotationBobOffset = 0.5f;
			Vector3 rotBobAmplitude = bob.GetRotationAmplitude() * 0.001f;

			m_StateRotation.x += std::cos(currentBobParam * 2 + rotationBobOffset) * rotBobAmplitude.x * mod;
			m_StateRotation.y += std::cos(currentBobParam + rotationBobOffset) * rotBobAmplitude.y * mod;
			m_StateRotation.z += std::cos(currentBobParam + rotationBobOffset) * rotBobAmplitude.z * mod;
		}
	}

	void PlayerCamera::UpdateNoise()
	{
		if (m_CurrentState->GetNoise().IsEnabled())
		{
			const NoiseModuleData& noise = m_Movement->IsAiming() ? m_AimState.GetNoise() : m_CurrentState->GetNoise();

			float jitter = 0;
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

	void PlayerCamera::UpdateSway()
	{
		float multiplier = (m_Movement->IsAiming() ? m_Sway.GetAimMultiplier() : 1.0f) * Time::GetFixedDeltaTime();

		Vector2 lookInput = m_LookAxis;
		lookInput *= m_Sway.GetLookInputMultiplier();
		lookInput = Math::ClampMagnitude(lookInput, m_Sway.GetMaxLookInput());

		Vector3 swayVelocity = m_Movement->GetVelocity();

		if (std::abs(swayVelocity.y) < 1.5f)
		{
			swayVelocity.y = 0.0f;
		}

		Vector3 localVelocity = Math::MultiplyVector(m_CameraTransform->GetWorldToLocalMatrix(), swayVelocity / 60.0f);

		Vector3 lookPositionSway = m_Sway.GetLookPositionSway();
		Vector3 lookRotationSway = m_Sway.GetLookRotationSway();
		Vector3 strafePositionSway = m_Sway.GetStrafePositionSway();
		Vector3 strafeRotationSway = m_Sway.GetStrafeRotationSway();

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

	void PlayerCamera::ApplyStepForce()
	{
	}

	void PlayerCamera::UpdateCameraTransform()
	{
		float targetSensitivity = 2.5f;
		float maxLookInput = 5.0f;

		Vector2 lookAxis = Math::ClampMagnitude(Input::GetMouseDelta() * 0.1f, maxLookInput);
		m_LookAxis = lookAxis;

		m_CurrentSensitivity = Math::Lerp(m_CurrentSensitivity, targetSensitivity, 8.0f * Time::GetDeltaTime());

		m_Rotation += lookAxis * m_CurrentSensitivity;
		m_Rotation.y = std::clamp(m_Rotation.y, -60.0f, 75.0f);

		m_PlayerRootTransform->SetLocalRotation(Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), Math::ToRadians(m_Rotation.x)));
		m_ViewRootTransform->SetLocalRotation(Quaternion::CreateFromAxisAngle(Vector3(1, 0, 0), Math::ToRadians(m_Rotation.y)));

		m_PositionSpring.Update();
		m_RotationSpring.Update();
		m_PositionRecoilSpring.Update();
		m_RotationRecoilSpring.Update();

		Vector3 localPosition = m_PositionSpring.GetPosition() + m_PositionRecoilSpring.GetPosition();
		Vector3 localRotation = m_RotationSpring.GetPosition() + m_RotationRecoilSpring.GetPosition();

		m_CameraTransform->SetLocalPosition(localPosition);
		m_CameraTransform->SetLocalEulerRotation(Vector3(Math::ToRadians(localRotation.x), Math::ToRadians(localRotation.y), Math::ToRadians(localRotation.z)));
	}

	void PlayerCamera::OnPlayerStep()
	{
		if (m_Movement->GetVelocity().LengthSquared() > 0.2f)
		{
			ApplyStepForce();
		}

		m_LastFootDown = !m_LastFootDown;
	}

	void PlayerCamera::OnPlayerJump()
	{
		m_PositionSpring.AddDistributedForce(m_Jump.GetPositionForce() / 100.0f, m_Jump.GetPositionDistribution());
		m_RotationSpring.AddDistributedForce(m_Jump.GetRotationForce() / 10.0f, m_Jump.GetRotationDistribution());
	}
}