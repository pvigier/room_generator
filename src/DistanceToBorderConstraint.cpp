#include "DistanceToBorderConstraint.h"
#include <algorithm>
#include "Grid.h"
#include "Variable.h"

namespace room
{

void DistanceToBorderConstraint::constrain(const Grid& grid, Variable& variable)
{
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    if (direction == Direction::North || direction == Direction::South)
    {
        auto i = direction == Direction::North ?
            distance - box.top :
            static_cast<int>(grid.getHeight()) - box.getBottom() - distance;
        std::erase_if(variable.domain, [i](const auto& value){ return value.y != i; });
    }
    else
    {
        auto j = direction == Direction::West ?
            distance - box.left :
            static_cast<int>(grid.getWidth()) - box.getRight() - distance;
        std::erase_if(variable.domain, [j](const auto& value){ return value.x != j; });
    }
}

}