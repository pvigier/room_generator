#pragma once

namespace room
{

class Grid;
struct Variable;

struct Constraint
{
    virtual ~Constraint() = default;

    virtual void constrain(const Grid& grid, Variable& variable) = 0;
};

}