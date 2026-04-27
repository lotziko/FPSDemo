#pragma once

#include "Blueberry\Scene\Components\Component.h"
#include "Blueberry\Core\ObjectPtr.h"
#include "Blueberry\Events\Event.h"
#include "Audio\SfxSound.h"

namespace Blueberry
{
	class CharacterController;
	class Transform;

	using PlayerStepEvent = Event<>;
	using PlayerStartRunningEvent = Event<>;
	using PlayerEndRunningEvent = Event<>;
	using PlayerJumpEvent = Event<>;

	class CoreMovementData
	{
	public:
		CoreMovementData() = default;
		CoreMovementData(float acceleration, float damping, float stepLength, float forwardSpeed, float backwardSpeed, float sideSpeed);

		float GetAcceleration();
		float GetDamping();
		float GetStepLength();
		float GetForwardSpeed();
		float GetBackwardSpeed();
		float GetSideSpeed();

	private:
		float m_Acceleration;
		float m_Damping;
		float m_StepLength;
		float m_ForwardSpeed;
		float m_BackwardSpeed;
		float m_SideSpeed;
	};

	class MovementStateData
	{
	public:
		MovementStateData() = default;
		MovementStateData(float speedMultiplier, float stepLength);

		float GetSpeedMultiplier();
		float GetStepLength();

	private:
		float m_SpeedMultiplier;
		float m_StepLength;
	};

	class PlayerMovement : public Component
	{
		OBJECT_DECLARATION(PlayerMovement)

	public:
		virtual void OnCreate() final;
		virtual void OnEnable() final;
		virtual void OnDisable() final;
		virtual void OnFixedUpdate() final;
		virtual void OnUpdate() final;

		const Vector3& GetVelocity();
		float GetMoveCycle();
		PlayerStepEvent& GetPlayerStepped();
		PlayerStartRunningEvent& GetPlayerStartedRunning();
		PlayerEndRunningEvent& GetPlayerEndedRunning();
		PlayerJumpEvent& GetPlayerJumped();

		bool IsWalking();
		bool IsRunning();
		bool IsAiming();

	private:
		void UpdateCharacterPhysics();
		Vector3 CalculateVelocity();
		void UpdateCharacterState();

	private:
		ObjectPtr<CharacterController> m_CharacterController;
		SfxSoundData m_StepSfx;
		SfxSoundData m_JumpSfx;
		
		CoreMovementData m_CoreMovement;
		MovementStateData m_RunState;

		ObjectPtr<Transform> m_Transform;
		MovementStateData* m_CurrentMovementState = nullptr;
		Vector3 m_DesiredVelocity;
		Vector3 m_CurrentVelocity;

		Vector2 m_MovementAxis;
		Vector3 m_DesiredVelocityLocal;
		bool m_IsWalking = false;
		bool m_IsRunning = false;
		bool m_IsJumping = false;
		bool m_IsAiming = false;

		float m_DistMovedSinceLastCycleEnded;
		float m_CurrentStepLength;
		float m_MoveCycle;

		PlayerStepEvent m_PlayerStepped;
		PlayerStartRunningEvent m_PlayerStartedRunning;
		PlayerEndRunningEvent m_PlayerEndedRunning;
		PlayerJumpEvent m_PlayerJumped;
	};
}