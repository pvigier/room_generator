#include "DistanceToWallConstraint.h"
#include <algorithm>
#include "Grid.h"
#include "Variable.h"

namespace room
{

void DistanceToWallConstraint::constrain(const Grid& grid, Variable& variable)
{
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    if (direction == Direction::North)
    {
        std::erase_if(variable.domain, [this, &grid, &box](const auto& value)
        {
            return !grid.isDistantToWall<Direction::North>(value.y, value.x, box, distance);
        });
    }
    else if (direction == Direction::West)
    {
        std::erase_if(variable.domain, [this, &grid, &box](const auto& value)
        {
            return !grid.isDistantToWall<Direction::West>(value.y, value.x, box, distance);
        });
    }
    else if (direction == Direction::South)
    {
        std::erase_if(variable.domain, [this, &grid, &box](const auto& value)
        {
            return !grid.isDistantToWall<Direction::South>(value.y, value.x, box, distance);
        });
    }
    else if (direction == Direction::East)
    {
        std::erase_if(variable.domain, [this, &grid, &box](const auto& value)
        {
            return !grid.isDistantToWall<Direction::East>(value.y, value.x, box, distance);
        });
    }
}

}