#pragma once

#include <algorithm>
namespace room
{

struct Box
{
    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;

    int getRight() const
    {
        return left + width;
    }

    int getBottom() const
    {
        return top + height;
    }

    Box getUnion(const Box& rhs) const
    {
        auto xMin = std::min(left, rhs.left);
        auto yMin = std::min(top, rhs.top);
        auto xMax = std::max(getRight(), rhs.getRight());
        auto yMax = std::max(getBottom(), rhs.getBottom());
        return Box{xMin, yMin, xMax - xMin, yMax - yMin};
    }
};

}