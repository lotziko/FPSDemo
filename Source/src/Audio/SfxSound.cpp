#include "SfxSound.h"

#include "Blueberry\Core\ClassDB.h"
#include "Blueberry\Audio\Audio.h"
#include "Blueberry\Audio\AudioClip.h"
#include "Blueberry\Core\Timer.h"

namespace Blueberry
{
	DATA_DEFINITION(SfxSoundData)
	{
		DEFINE_FIELD(SfxSoundData, m_Clips, BindingType::ObjectPtrList, FieldOptions().SetObjectType(&AudioClip::Type))
		DEFINE_FIELD(SfxSoundData, m_VolumeRange, BindingType::Vector2, FieldOptions())
		DEFINE_FIELD(SfxSoundData, m_PitchRange, BindingType::Vector2, FieldOptions())
		DEFINE_FIELD(SfxSoundData, m_VolumeMultiplier, BindingType::Float, FieldOptions())
	}

	DATA_DEFINITION(DelayedSfxSoundData)
	{
		DEFINE_FIELD(DelayedSfxSoundData, m_Clips, BindingType::ObjectPtrList, FieldOptions().SetObjectType(&AudioClip::Type))
		DEFINE_FIELD(DelayedSfxSoundData, m_VolumeRange, BindingType::Vector2, FieldOptions())
		DEFINE_FIELD(DelayedSfxSoundData, m_PitchRange, BindingType::Vector2, FieldOptions())
		DEFINE_FIELD(DelayedSfxSoundData, m_VolumeMultiplier, BindingType::Float, FieldOptions())
		DEFINE_FIELD(DelayedSfxSoundData, m_Delay, BindingType::Float, FieldOptions())
	}

	void SfxSoundData::Play()
	{
		if (m_Clips.size() == 0)
		{
			return;
		}
		Audio::Play(m_Clips[Math::GetRandomInt(0, static_cast<int>(m_Clips.size()))].Get(), Math::GetRandomFloat(m_VolumeRange.x, m_VolumeRange.y) * m_VolumeMultiplier, Math::GetRandomFloat(m_PitchRange.x, m_PitchRange.y));
	}

	void DelayedSfxSoundData::Play(Object* caller)
	{
		if (m_Clips.size() == 0)
		{
			return;
		}
		Timer::Start(m_Delay, caller, [this]()
		{
			Audio::Play(m_Clips[Math::GetRandomInt(0, static_cast<int>(m_Clips.size()))].Get(), Math::GetRandomFloat(m_VolumeRange.x, m_VolumeRange.y) * m_VolumeMultiplier, Math::GetRandomFloat(m_PitchRange.x, m_PitchRange.y));
		});
	}
}