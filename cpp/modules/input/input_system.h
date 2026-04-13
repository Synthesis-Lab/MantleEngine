#ifndef MANTLE_INPUT_SYSTEM_H
#define MANTLE_INPUT_SYSTEM_H

#include "core/module.h"
#include <array>
#include <cstdint>

class InputSystem final : public Module {
public:
    enum class KeyState : uint8_t {
        Up = 0,
        Pressed,
        Held,
        Released
    };

    bool Initialize() override;
    void Update(float delta_time) override;
    void Shutdown() override;
    const char* GetName() const override;

    void SetKeyDown(uint8_t key, bool is_down);
    KeyState GetKeyState(uint8_t key) const;

private:
    std::array<KeyState, 256> key_states_{};
    std::array<bool, 256> key_down_{};
};

#endif // MANTLE_INPUT_SYSTEM_H

