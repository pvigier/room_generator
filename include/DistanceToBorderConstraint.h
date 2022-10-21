#pragma once

#include "Constraint.h"
#include "Direction.h"

namespace room
{

struct DistanceToBorderConstraint : public Constraint
{
    Direction direction = Direction::North;
    int distance = 0;

    void constrain(const Grid& grid, Variable& variable) final;
};

}