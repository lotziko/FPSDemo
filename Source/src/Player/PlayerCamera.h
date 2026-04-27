#pragma once

#include "Blueberry\Scene\Components\Component.h"
#include "Blueberry\Core\ObjectPtr.h"
#include "Blueberry\Events\Event.h"
#include "Physics\Spring.h"
#include "Physics\Modules.h"

namespace Blueberry
{
	class PlayerMovement;
	class Camera;

	class CameraMotionStateData : public Data
	{
	public:
		CameraMotionStateData() = default;
		CameraMotionStateData(const BobModuleData& bob, const NoiseModuleData& noise);

		const BobModuleData& GetBob();
		const NoiseModuleData& GetNoise();

	private:
		BobModuleData m_Bob;
		NoiseModuleData m_Noise;
	};

	class PlayerCamera : public Component
	{
		OBJECT_DECLARATION(PlayerCamera)
		
	public:
		virtual void OnCreate() final;
		virtual void OnEnable() final;
		virtual void OnDisable() final;
		virtual void OnFixedUpdate() final;
		virtual void OnUpdate() final;

		const Vector2& GetLookAxis() const;

		const SwayModuleData& GetSway() const;

		void AddPositionRecoilForce(const Vector3& positionForce, int distribution = 1);
		void AddRotationRecoilForce(const Vector3& positionForce, int distribution = 1);
		void MoveCamera(const Vector2& movement);
		void SetTargetFov(float targetFov);

	private:
		void UpdateCameraPhysics();
		void UpdateState();
		void UpdateFov();
		void UpdateStationaryBob(float deltaTime);
		void UpdateMovementBob(float deltaTime);
		void UpdateBob(float currentBobParam, CameraMotionStateData* state, float mod = 1.0f);
		void UpdateNoise();
		void UpdateSway();
		void ApplyStepForce();

		void UpdateCameraTransform();

		void OnPlayerStep();
		void OnPlayerJump();

	private:
		ObjectPtr<Transform> m_PlayerRootTransform;
		ObjectPtr<Transform> m_ViewRootTransform;
		ObjectPtr<Transform> m_CameraTransform;

		ObjectPtr<PlayerMovement> m_Movement;
		ObjectPtr<Camera> m_Camera;
		Spring m_PositionSpring;
		Spring m_RotationSpring;
		Spring m_PositionRecoilSpring;
		Spring m_RotationRecoilSpring;

		CameraMotionStateData* m_CurrentState = nullptr;
		SwayModuleData m_Sway;
		JumpModuleData m_Jump;
		CameraMotionStateData m_IdleState;
		CameraMotionStateData m_WalkState;
		CameraMotionStateData m_RunState;
		CameraMotionStateData m_AimState;

		Vector2 m_Rotation;
		Vector2 m_LookAxis;
		float m_CurrentStaticBobParam;
		float m_CurrentMovingBobParam;
		float m_CurrentSensitivity;
		bool m_LastFootDown;
		Vector3 m_StatePosition;
		Vector3 m_StateRotation;
		float m_TargetFov = 0.0f;
	};
}