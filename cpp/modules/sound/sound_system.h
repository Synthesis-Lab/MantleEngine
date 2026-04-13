#ifndef MANTLE_SOUND_SYSTEM_H
#define MANTLE_SOUND_SYSTEM_H

#include "core/module.h"
#include <string>
#include <unordered_map>

class SoundSystem final : public Module {
public:
    bool Initialize() override;
    void Update(float delta_time) override;
    void Shutdown() override;
    const char* GetName() const override;

    int LoadClip(const std::string& name);
    void Play(int clip_id);
    void Stop(int clip_id);
    bool IsPlaying(int clip_id) const;

private:
    int next_clip_id_ = 1;
    std::unordered_map<int, bool> playing_;
};

#endif // MANTLE_SOUND_SYSTEM_H

