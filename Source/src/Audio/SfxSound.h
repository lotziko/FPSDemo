#pragma once

#include "Blueberry\Core\Base.h"
#include "Blueberry\Core\Object.h"
#include "Blueberry\Core\ObjectPtr.h"

namespace Blueberry
{
	class AudioClip;

	class SfxSoundData : public Data
	{
		DATA_DECLARATION(SfxSoundData)

	public:
		SfxSoundData() = default;

		void Play();

	private:
		List<ObjectPtr<AudioClip>> m_Clips;
		Vector2 m_VolumeRange = Vector2(1.0f, 1.0f);
		Vector2 m_PitchRange = Vector2(1.0f, 1.0f);
		float m_VolumeMultiplier = 1.0f;
	};

	class DelayedSfxSoundData : public Data
	{
		DATA_DECLARATION(SfxSoundData)

	public:
		DelayedSfxSoundData() = default;

		void Play(Object* caller);

	private:
		List<ObjectPtr<AudioClip>> m_Clips;
		Vector2 m_VolumeRange = Vector2(1.0f, 1.0f);
		Vector2 m_PitchRange = Vector2(1.0f, 1.0f);
		float m_VolumeMultiplier = 1.0f;
		float m_Delay = 0.0f;
	};
}