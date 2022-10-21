#pragma once

#include <cstdint>

namespace room
{

enum class Direction : uint8_t
{
    North,
    West,
    South,
    East,
};

inline constexpr auto DirectionCount = 4;

}
