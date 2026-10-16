#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX

#endif
#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace HookKit {
class ScopedUnprotect {
public:
    ScopedUnprotect(void* address, std::size_t size)
        : address_(address),
          size_(size),
          ok_(VirtualProtect(address_, size_, PAGE_EXECUTE_READWRITE, &old_protect_) != 0) {}

    ~ScopedUnprotect() {
        if (!ok_) { return; }

        DWORD temp = 0;
        VirtualProtect(address_, size_, old_protect_, &temp);
    }

    ScopedUnprotect(const ScopedUnprotect&) = delete;
    ScopedUnprotect& operator=(const ScopedUnprotect&) = delete;

    ScopedUnprotect(ScopedUnprotect&&) = delete;
    ScopedUnprotect& operator=(ScopedUnprotect&&) = delete;

    [[nodiscard]]
    bool Ok() const noexcept {
        return ok_;
    }
    [[nodiscard]]
    bool ok() const noexcept {
        return Ok();
    }
    [[nodiscard]]
    bool OK() const noexcept {
        return Ok();
    }

private:
    void* address_;
    std::size_t size_;
    DWORD old_protect_ = 0;
    bool ok_ = false;
};

template <typename T>
bool PatchBytes(void* address, const T* data, std::size_t size) {
    ScopedUnprotect guard(address, size);
    if (!guard.Ok()) { return false; }

    std::memcpy(address, data, size);
    FlushInstructionCache(GetCurrentProcess(), address, size);
    return true;
}

template <typename T>
bool patchBytes(void* address, const T* data, std::size_t size) {
    return PatchBytes(address, data, size);
}

template <typename T>
bool ForceWrite(std::uintptr_t address, const T& value) {
    return PatchBytes(reinterpret_cast<void*>(address), &value, sizeof(T));
}

template <typename T>
bool forceWrite(std::uintptr_t address, const T& value) {
    return ForceWrite(address, value);
}

// SCREAMING_SNAKE_CASE Aliases
template <typename T>
bool PATCH_BYTES(void* address, const T* data, std::size_t size) {
    return PatchBytes(address, data, size);
}

template <typename T>
bool FORCE_WRITE(std::uintptr_t address, const T& value) {
    return ForceWrite(address, value);
}

}  // namespace HookKit
namespace hookkit = HookKit;
