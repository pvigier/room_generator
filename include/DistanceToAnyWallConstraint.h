#pragma once

#include <cstdint>
#include "Constraint.h"

namespace room
{

struct DistanceToAnyWallConstraint : public Constraint
{
    uint32_t distance = 0;

    void constrain(const Grid& grid, Variable& variable) final;
};

}