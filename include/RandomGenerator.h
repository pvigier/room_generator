#pragma once

#include <random>

namespace room
{

// Use a better random generator in production
using RandomGenerator = std::mt19937;

}