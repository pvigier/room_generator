#include "NextToDoorConstraint.h"
#include <algorithm>
#include "Grid.h"
#include "Variable.h"

namespace room
{

void NextToDoorConstraint::constrain(const Grid& grid, Variable& variable)
{
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    if (side == Direction::North || side == Direction::South)
    {
        auto offset = side == Direction::North ? box.top : box.getBottom() - 1;
        std::erase_if(variable.domain, [&grid, &variable, offset](const auto& value)
        {
            auto i = value.y + offset;
            auto j1 = value.x + variable.collisionBox.left - 1;
            auto j2 = value.x + variable.collisionBox.getRight();
            return (j1 < 0 || !grid.isDoor(i, j1)) &&
                (static_cast<std::size_t>(j2) >= grid.getWidth() || !grid.isDoor(i, j2));
        });
    }
    else
    {
        auto offset = side == Direction::West ? box.left : box.getRight() - 1;
        std::erase_if(variable.domain, [&grid, &variable, offset](const auto& value)
        {
            auto j = value.x + offset;
            auto i1 = value.y + variable.collisionBox.top - 1;
            auto i2 = value.y + variable.collisionBox.getBottom();
            return (i1 < 0 || !grid.isDoor(i1, j)) &&
                (static_cast<std::size_t>(i2) >= grid.getHeight() || !grid.isDoor(i2, j));
        });
    }
}

}