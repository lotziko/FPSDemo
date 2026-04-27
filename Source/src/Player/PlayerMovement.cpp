#include "PlayerMovement.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Core\Time.h"
#include "Blueberry\Scene\Components\CharacterController.h"
#include "Blueberry\Scene\Components\Transform.h"
#include "Blueberry\Input\Input.h"

namespace Blueberry
{
	OBJECT_DEFINITION(PlayerMovement, Component)
	{
		DEFINE_BASE_FIELDS(PlayerMovement, Component)
		DEFINE_FIELD(PlayerMovement, m_CharacterController, BindingType::ObjectPtr, FieldOptions().SetObjectType(&CharacterController::Type))
		DEFINE_FIELD(PlayerMovement, m_StepSfx, BindingType::Data, FieldOptions().SetObjectType(&SfxSoundData::Type))
		DEFINE_FIELD(PlayerMovement, m_JumpSfx, BindingType::Data, FieldOptions().SetObjectType(&SfxSoundData::Type))
		DEFINE_ITERATOR(UpdatableComponent)
	}

	CoreMovementData::CoreMovementData(float acceleration, float damping, float stepLength, float forwardSpeed, float backwardSpeed, float sideSpeed) : m_Acceleration(acceleration), m_Damping(damping), m_StepLength(stepLength), m_ForwardSpeed(forwardSpeed), m_BackwardSpeed(backwardSpeed), m_SideSpeed(sideSpeed)
	{
	}

	float CoreMovementData::GetAcceleration()
	{
		return m_Acceleration;
	}

	float CoreMovementData::GetDamping()
	{
		return m_Damping;
	}

	float CoreMovementData::GetStepLength()
	{
		return m_StepLength;
	}

	float CoreMovementData::GetForwardSpeed()
	{
		return m_ForwardSpeed;
	}

	float CoreMovementData::GetBackwardSpeed()
	{
		return m_BackwardSpeed;
	}

	float CoreMovementData::GetSideSpeed()
	{
		return m_SideSpeed;
	}

	MovementStateData::MovementStateData(float speedMultiplier, float stepLength) : m_SpeedMultiplier(speedMultiplier), m_StepLength(stepLength)
	{
	}

	float MovementStateData::GetSpeedMultiplier()
	{
		return m_SpeedMultiplier;
	}

	float MovementStateData::GetStepLength()
	{
		return m_StepLength;
	}

	void PlayerMovement::OnCreate()
	{
		m_Transform = GetTransform();
		m_CoreMovement = CoreMovementData(9.0f, 10.0f, 1.3f, 2.75f, 2.25f, 2.6f);
		m_RunState = MovementStateData(5.5f, 1.95f);
	}

	void PlayerMovement::OnEnable()
	{
	}

	void PlayerMovement::OnDisable()
	{
	}

	void PlayerMovement::OnFixedUpdate()
	{
		UpdateCharacterPhysics();
	}

	void PlayerMovement::OnUpdate()
	{
		UpdateCharacterState();
	}

	const Vector3& PlayerMovement::GetVelocity()
	{
		return m_CharacterController->GetVelocity();
	}

	float PlayerMovement::GetMoveCycle()
	{
		return m_MoveCycle;
	}

	PlayerStepEvent& PlayerMovement::GetPlayerStepped()
	{
		return m_PlayerStepped;
	}

	PlayerStartRunningEvent& PlayerMovement::GetPlayerStartedRunning()
	{
		return m_PlayerStartedRunning;
	}

	PlayerEndRunningEvent& PlayerMovement::GetPlayerEndedRunning()
	{
		return m_PlayerEndedRunning;
	}

	PlayerJumpEvent& PlayerMovement::GetPlayerJumped()
	{
		return m_PlayerJumped;
	}

	bool PlayerMovement::IsWalking()
	{
		return m_IsWalking;
	}

	bool PlayerMovement::IsRunning()
	{
		return m_IsRunning;
	}

	bool PlayerMovement::IsAiming()
	{
		return m_IsAiming;
	}

	void PlayerMovement::UpdateCharacterPhysics()
	{
		float deltaTime = Time::GetFixedDeltaTime();
		Vector3 targetVelocity = CalculateVelocity();
		float targetAcceleration = targetVelocity.LengthSquared() > 0.0f ? m_CoreMovement.GetAcceleration() : m_CoreMovement.GetDamping();
		m_DesiredVelocity = Vector3::Lerp(m_DesiredVelocity, targetVelocity, targetAcceleration * deltaTime);

		Quaternion rotation = m_Transform->GetRotation();
		Vector3 forward = Vector3::Transform(Vector3(0, 0, 1), rotation);
		Vector3 right = Vector3::Transform(Vector3(1, 0, 0), rotation);
		m_CurrentVelocity = Vector3(right * m_DesiredVelocity.x + forward * m_DesiredVelocity.z);

		m_DistMovedSinceLastCycleEnded += m_DesiredVelocity.Length() * deltaTime;
		
		float targetStepLength = m_CoreMovement.GetStepLength();
		if (m_CurrentMovementState != nullptr)
		{
			targetStepLength = m_CurrentMovementState->GetStepLength();
		}
		m_CurrentStepLength = Math::MoveTowards(m_CurrentStepLength, targetStepLength, deltaTime * 0.6f);
		if (m_DistMovedSinceLastCycleEnded > m_CurrentStepLength)
		{
			m_DistMovedSinceLastCycleEnded -= m_CurrentStepLength;
			m_PlayerStepped.Invoke();
			if (m_CharacterController->IsGrounded())
			{
				m_StepSfx.Play();
			}
		}
		m_MoveCycle = m_DistMovedSinceLastCycleEnded / m_CurrentStepLength;
		if (m_IsJumping)
		{
			m_IsJumping = false;
			m_CurrentVelocity.y += 4.5f;
			m_PlayerJumped.Invoke();
			m_JumpSfx.Play();
		}
		m_CharacterController->Move(m_CurrentVelocity);
	}

	Vector3 PlayerMovement::CalculateVelocity()
	{
		bool wantsToMove = m_MovementAxis.LengthSquared() > 0.0f;
		Vector3 targetDirection = wantsToMove ? Vector3(m_MovementAxis.x, 0, m_MovementAxis.y) : m_DesiredVelocityLocal;
		float desiredSpeed = 0.0f;

		if (wantsToMove)
		{
			desiredSpeed = m_CoreMovement.GetForwardSpeed();

			if (std::abs(m_MovementAxis.x) > 0.0f)
			{
				desiredSpeed = m_CoreMovement.GetSideSpeed();
			}

			if (m_MovementAxis.y < 0.0f)
			{
				desiredSpeed = m_CoreMovement.GetBackwardSpeed();
			}

			if (m_IsRunning)
			{
				if (desiredSpeed == m_CoreMovement.GetForwardSpeed() || desiredSpeed == m_CoreMovement.GetSideSpeed())
				{
					desiredSpeed = m_CurrentMovementState->GetSpeedMultiplier();
				}
			}
			else
			{

			}
		}

		return targetDirection * desiredSpeed;
	}

	void PlayerMovement::UpdateCharacterState()
	{
		m_MovementAxis = Vector2(static_cast<float>(Input::IsKeyDown(KeyCode::D) ? 1 : 0 + Input::IsKeyDown(KeyCode::A) ? -1 : 0), static_cast<float>(Input::IsKeyDown(KeyCode::W) ? 1 : 0 + Input::IsKeyDown(KeyCode::S) ? -1 : 0));
		m_MovementAxis.Normalize();
		bool isWalking = m_MovementAxis.LengthSquared() > 0.0f;
		bool isRunning = Input::IsKeyDown(KeyCode::Shift) && !m_IsAiming && m_MovementAxis.y > 0.0f;
		m_IsJumping = Input::IsKeyPressed(KeyCode::Space) && m_CharacterController->IsGrounded() && m_CharacterController->GetGroundNormal().Dot(Vector3(0, 1, 0)) > 0.75f;
		m_IsAiming = Input::IsKeyDown(KeyCode::MouseRight) && !isRunning;

		if (isRunning)
		{
			m_CurrentMovementState = &m_RunState;
			isWalking = false;
		}
		else
		{
			m_CurrentMovementState = nullptr;
		}

		if (isWalking != m_IsWalking)
		{
			m_IsWalking = isWalking;
		}
		if (isRunning != m_IsRunning)
		{
			m_IsRunning = isRunning;
			if (isRunning)
			{
				m_PlayerStartedRunning.Invoke();
			}
			else
			{
				m_PlayerEndedRunning.Invoke();
			}
		}
	}
}