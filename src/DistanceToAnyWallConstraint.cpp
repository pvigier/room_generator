#include "DistanceToAnyWallConstraint.h"
#include <algorithm>
#include "Grid.h"
#include "Variable.h"

namespace room
{

void DistanceToAnyWallConstraint::constrain(const Grid& grid, Variable& variable)
{
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    std::erase_if(variable.domain, [this, &grid, &box](const auto& value)
    {
        return !grid.isDistantToWall<Direction::North>(value.y, value.x, box, distance) &&
            !grid.isDistantToWall<Direction::West>(value.y, value.x, box, distance) &&
            !grid.isDistantToWall<Direction::South>(value.y, value.x, box, distance) &&
            !grid.isDistantToWall<Direction::East>(value.y, value.x, box, distance);
    });
}

}