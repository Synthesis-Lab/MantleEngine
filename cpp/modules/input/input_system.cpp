#include "modules/input/input_system.h"

bool InputSystem::Initialize() {
    key_states_.fill(KeyState::Up);
    key_down_.fill(false);
    return true;
}

void InputSystem::Update(float /*delta_time*/) {
    for (std::size_t i = 0; i < key_states_.size(); ++i) {
        const bool down = key_down_[i];
        const KeyState prev = key_states_[i];

        if (down) {
            key_states_[i] = (prev == KeyState::Up || prev == KeyState::Released)
                ? KeyState::Pressed
                : KeyState::Held;
        } else {
            key_states_[i] = (prev == KeyState::Pressed || prev == KeyState::Held)
                ? KeyState::Released
                : KeyState::Up;
        }
    }
}

void InputSystem::Shutdown() {
    key_states_.fill(KeyState::Up);
    key_down_.fill(false);
}

const char* InputSystem::GetName() const {
    return "InputSystem";
}

void InputSystem::SetKeyDown(uint8_t key, bool is_down) {
    const std::size_t idx = static_cast<std::size_t>(key);
    if (idx >= key_down_.size()) {
        return;
    }
    key_down_[idx] = is_down;
}

InputSystem::KeyState InputSystem::GetKeyState(uint8_t key) const {
    const std::size_t idx = static_cast<std::size_t>(key);
    if (idx >= key_states_.size()) {
        return KeyState::Up;
    }
    return key_states_[idx];
}

