#pragma once

#include "Constraint.h"
#include "Direction.h"

namespace room
{

struct DistanceToWallConstraint : public Constraint
{
    Direction direction = Direction::North;
    uint32_t distance = 0;

    void constrain(const Grid& grid, Variable& variable) final;
};

}