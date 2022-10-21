#include "CornerConstraint.h"
#include <algorithm>
#include "Grid.h"
#include "Variable.h"

namespace room
{

void CornerConstraint::constrain(const Grid& grid, Variable& variable)
{
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    std::erase_if(variable.domain, [&grid, &box](const auto& value)
    {
        return !grid.isInCorner(value.y, value.x, box);
    });
}

}