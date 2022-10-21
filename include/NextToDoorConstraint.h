#pragma once

#include "Constraint.h"
#include "Direction.h"

namespace room
{

struct NextToDoorConstraint : public Constraint
{
    Direction side = Direction::North;

    void constrain(const Grid& grid, Variable& variable) final;
};

}