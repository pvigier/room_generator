#pragma once

#include "Constraint.h"

namespace room
{

struct CornerConstraint : public Constraint
{
    void constrain(const Grid& grid, Variable& variable) final;
};

}