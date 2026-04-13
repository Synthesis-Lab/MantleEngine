#include "modules/sound/sound_system.h"

bool SoundSystem::Initialize() {
    playing_.clear();
    next_clip_id_ = 1;
    return true;
}

void SoundSystem::Update(float /*delta_time*/) {
}

void SoundSystem::Shutdown() {
    playing_.clear();
}

const char* SoundSystem::GetName() const {
    return "SoundSystem";
}

int SoundSystem::LoadClip(const std::string& /*name*/) {
    const int id = next_clip_id_++;
    playing_[id] = false;
    return id;
}

void SoundSystem::Play(int clip_id) {
    if (playing_.count(clip_id) != 0U) {
        playing_[clip_id] = true;
    }
}

void SoundSystem::Stop(int clip_id) {
    if (playing_.count(clip_id) != 0U) {
        playing_[clip_id] = false;
    }
}

bool SoundSystem::IsPlaying(int clip_id) const {
    const auto it = playing_.find(clip_id);
    return it != playing_.end() && it->second;
}

