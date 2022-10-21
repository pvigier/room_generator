#include "Grid.h"
#include <functional>
#include <limits>
#include <queue>
#include "Variable.h"

using namespace room;

namespace room
{

Grid::Grid(const Array2<Cell>& cells, const std::vector<Grid::Door>& doors, bool connected) :
    mCells(cells.getHeight(), cells.getWidth()),
    mDoors(cells.getHeight(), cells.getWidth(), false),
    mConnected(connected)
{
    // Set up cells
    for (auto i = std::size_t(0); i < cells.getHeight(); ++i)
    {
        for (auto j = std::size_t(0); j < cells.getWidth(); ++j)
            mCells.get(i, j) = static_cast<int8_t>(cells.get(i, j));
    }
    // Set doors
    for (const auto& door : doors)
        mDoors.get(static_cast<std::size_t>(door.position.y), static_cast<std::size_t>(door.position.x)) = true;
    // Compute distance to walls
    static constexpr auto MaxDistance = std::numeric_limits<uint32_t>::max();
    getDistancesToWall(Direction::North).reshape(cells.getHeight(), cells.getWidth());
    for (auto j = std::size_t(0); j < cells.getWidth(); ++j)
    {
        auto distance = MaxDistance;
        for (auto i = std::size_t(0); i < cells.getHeight(); ++i)
        {
            if (isWall(i, j))
            {
                getDistancesToWall(Direction::North).get(i, j) = MaxDistance;
                distance = 0;
            }
            else
            {
                getDistancesToWall(Direction::North).get(i, j) = distance;
                distance += distance != MaxDistance;
            }
        }
    }
    getDistancesToWall(Direction::West).reshape(cells.getHeight(), cells.getWidth());
    for (auto i = std::size_t(0); i < cells.getHeight(); ++i)
    {
        auto distance = MaxDistance;
        for (auto j = std::size_t(0); j < cells.getWidth(); ++j)
        {
            if (isWall(i, j))
            {
                getDistancesToWall(Direction::West).get(i, j) = MaxDistance;
                distance = 0;
            }
            else
            {
                getDistancesToWall(Direction::West).get(i, j) = distance;
                distance += distance != MaxDistance;
            }
        }
    }
    getDistancesToWall(Direction::South).reshape(cells.getHeight(), cells.getWidth());
    for (auto j = std::size_t(0); j < cells.getWidth(); ++j)
    {
        auto distance = MaxDistance;
        for (auto i = static_cast<int>(cells.getHeight()) - 1; i >= 0; --i)
        {
            auto y = static_cast<std::size_t>(i);
            if (isWall(y, j))
            {
                getDistancesToWall(Direction::South).get(y, j) = MaxDistance;
                distance = 0;
            }
            else
            {
                getDistancesToWall(Direction::South).get(y, j) = distance;
                distance += distance != MaxDistance;
            }
        }
    }
    getDistancesToWall(Direction::East).reshape(cells.getHeight(), cells.getWidth());
    for (auto i = std::size_t(0); i < cells.getHeight(); ++i)
    {
        auto distance = MaxDistance;
        for (auto j = static_cast<int>(cells.getWidth()) - 1; j >= 0; --j)
        {
            auto x = static_cast<std::size_t>(j);
            if (isWall(i, x))
            {
                getDistancesToWall(Direction::East).get(i, x) = MaxDistance;
                distance = 0;
            }
            else
            {
                getDistancesToWall(Direction::East).get(i, x) = distance;
                distance += distance != MaxDistance;
            }
        }
    }
}

std::size_t Grid::getHeight() const
{
    return mCells.getHeight();
}

std::size_t Grid::getWidth() const
{
    return mCells.getWidth();
}

bool Grid::isDoor(int i, int j) const
{
    return mDoors.get(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
}

template<Direction Direction>
bool Grid::isDistantToWall(int i, int j, const Box& box, uint32_t distance) const
{
    auto minDistance = std::numeric_limits<uint32_t>::max();
    if constexpr (Direction == Direction::North)
    {
        auto y = static_cast<std::size_t>(i + box.top);
        auto xStart = static_cast<std::size_t>(j + box.left);
        auto xEnd = static_cast<std::size_t>(j + box.getRight());
        for (auto x = xStart; x < xEnd; ++x)
            minDistance = std::min(getDistancesToWall(Direction).get(y, x), minDistance);
    }
    else if constexpr (Direction == Direction::West)
    {
        auto yStart = static_cast<std::size_t>(i + box.top);
        auto yEnd = static_cast<std::size_t>(i + box.getBottom());
        auto x = static_cast<std::size_t>(j + box.left);
        for (auto y = yStart; y < yEnd; ++y)
            minDistance = std::min(getDistancesToWall(Direction).get(y, x), minDistance);
    }
    else if constexpr (Direction == Direction::South)
    {
        auto y = static_cast<std::size_t>(i + box.getBottom() - 1);
        auto xStart = static_cast<std::size_t>(j + box.left);
        auto xEnd = static_cast<std::size_t>(j + box.getRight());
        for (auto x = xStart; x < xEnd; ++x)
            minDistance = std::min(getDistancesToWall(Direction).get(y, x), minDistance);
    }
    else if constexpr (Direction == Direction::East)
    {
        auto yStart = static_cast<std::size_t>(i + box.top);
        auto yEnd = static_cast<std::size_t>(i + box.getBottom());
        auto x = static_cast<std::size_t>(j + box.getRight() - 1);
        for (auto y = yStart; y < yEnd; ++y)
            minDistance = std::min(getDistancesToWall(Direction).get(y, x), minDistance);
    }
    return minDistance == distance;
}

bool Grid::isInCorner(int i, int j, const Box& box) const
{
    auto [yStart, yEnd, xStart, xEnd] = getCorners(i, j, box);
    return getDistancesToWall(Direction::North).get(yStart, xStart) + getDistancesToWall(Direction::West).get(yStart, xStart) == 0 ||
        getDistancesToWall(Direction::North).get(yStart, xEnd - 1) + getDistancesToWall(Direction::East).get(yStart, xEnd - 1) == 0 ||
        getDistancesToWall(Direction::South).get(yEnd - 1, xStart) + getDistancesToWall(Direction::West).get(yEnd - 1, xStart) == 0 ||
        getDistancesToWall(Direction::South).get(yEnd - 1, xEnd - 1) + getDistancesToWall(Direction::East).get(yEnd - 1, xEnd - 1) == 0;
}

bool Grid::tryPlace(int i, int j, const Variable& variable)
{
    // Check that the collision box is free
    const auto& collisionCorners = getCorners(i, j, variable.collisionBox);
    if (!checkInBox(collisionCorners, std::bind_front(&Grid::isEmpty, this)))
        return false;
    // Check that the margin box is free
    // MAYBE: do not check indices inside the collision box
    auto marginCorners = getCorners(i, j, variable.marginBox);
    if (!checkInBox(marginCorners, std::bind_front(&Grid::isEmptyOrMargin, this)))
        return false;
    // Add the variable
    applyInBox(marginCorners, std::bind_front(&Grid::addMargin, this));
    applyInBox(collisionCorners, std::bind_front(&Grid::setFull, this));
    // Check that adding the box does not disconnect the grid
    const auto& [yStart, yEnd, xStart, xEnd] = collisionCorners;
    if (mConnected &&
        mayDisconnect(static_cast<int>(yStart), static_cast<int>(yEnd), static_cast<int>(xStart), static_cast<int>(xEnd)) &&
        !isConnected())
    {
        remove(i, j, variable);
        return false;
    }
    return true;
}

void Grid::remove(int i, int j, const Variable& variable)
{
    // Set empty
    const auto& collisionCorners = getCorners(i, j, variable.collisionBox);
    applyInBox(collisionCorners, std::bind_front(&Grid::setEmpty, this));
    // Remove margins
    applyInBox(getCorners(i, j, variable.marginBox), std::bind_front(&Grid::removeMargin, this));
}

bool Grid::isConnected() const
{
    static constexpr auto deltas = std::array
    {
        Vector2i{0, -1}, Vector2i{-1, 0}, Vector2i{1, 0}, Vector2i{0, 1}
    };
    auto reachable = Array2<bool>(getHeight(), getWidth(), false);
    auto frontier = std::queue<Vector2i>();
    // Find an empty or margin cell to use as start of the BFS
    auto found = false;
    for (auto i = std::size_t(0); i < getHeight() && !found; ++i)
    {
        for (auto j = std::size_t(0); j < getWidth() && !found; ++j)
        {
            if (isEmptyOrMargin(i, j))
            {
                reachable.get(i, j) = true;
                frontier.push(Vector2i{static_cast<int>(j), static_cast<int>(i)});
                found = true;
            }
        }
    }
    // BFS
    while (!frontier.empty())
    {
        auto cell = frontier.front();
        frontier.pop();
        for (const auto& delta : deltas)
        {
            auto i = static_cast<std::size_t>(cell.y + delta.y);
            auto j = static_cast<std::size_t>(cell.x + delta.x);
            if (i < reachable.getHeight() && j < reachable.getWidth() &&
                !reachable.get(i, j) && isEmptyOrMargin(i, j))
            {
                reachable.get(i, j) = true;
                frontier.push(Vector2i{static_cast<int>(j), static_cast<int>(i)});
            }
        }
    }
    // Check that all empty or margin cells are reachable
    for (auto i = std::size_t(0); i < getHeight(); ++i)
    {
        for (auto j = std::size_t(0); j < getWidth(); ++j)
        {
            if (isEmptyOrMargin(i, j) && !reachable.get(i, j))
                return false;
        }
    }
    return true;
}

Array2<Grid::Cell> Grid::getCells() const
{
    auto cells = Array2<Grid::Cell>(getHeight(), getWidth());
    for (auto i = std::size_t(0); i < getHeight(); ++i)
    {
        for (auto j = std::size_t(0); j < getWidth(); ++j)
        {
            if (isWall(i, j))
                cells.get(i, j) = Cell::Wall;
            else if (isFull(i, j))
                cells.get(i, j) = Cell::Full;
            else if (isEmpty(i, j))
                cells.get(i, j) = Cell::Empty;
            else
                cells.get(i, j) = Cell::Margin;
        }
    }
    return cells;
}

bool Grid::mayDisconnect(int iStart, int iEnd, int jStart, int jEnd) const
{
    // This function checks the frontier of the box, if frontier is connected, the changes inside the box will leave the room connected
    // To check the connectedness, we compute the number of distinct walls in the frontier
    auto getState = [this](std::size_t y, std::size_t x){ return y >= getHeight() || x >= getWidth() || isFull(y, x); };
    auto i = iStart - 1;
    auto j = jStart - 1;
    auto state = getState(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
    auto nbWalls = 0;
    auto updateState = [&getState, &state, &nbWalls, &i, &j]()
    {
        auto newState = getState(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
        nbWalls += static_cast<int>(!state && newState); // We detect the beginning of a wall
        state = newState;
    };
    for (j = jStart; j <= jEnd; ++j)
        updateState();
    for (i = iStart, j = jEnd; i <= iEnd; ++i)
        updateState();
    for (i = iEnd, j = jEnd - 1; j >= jStart - 1; --j)
        updateState();
    for (i = iEnd - 1, j = jStart - 1; i >= iStart - 1; --i)
        updateState();
    return nbWalls > 1;
}

bool Grid::isEmpty(std::size_t i, std::size_t j) const
{
    return mCells.get(i, j) == 0;
}

bool Grid::isEmptyOrMargin(std::size_t i, std::size_t j) const
{
    return mCells.get(i, j) >= 0;
}

bool Grid::isFull(std::size_t i, std::size_t j) const
{
    return mCells.get(i, j) < 0;
}

bool Grid::isWall(std::size_t i, std::size_t j) const
{
    return mCells.get(i, j) == static_cast<int8_t>(Cell::Wall);
}

void Grid::setEmpty(std::size_t i, std::size_t j)
{
    assert(mCells.get(i, j) == static_cast<int8_t>(Grid::Cell::Full));
    mCells.get(i, j) = 0;
}

void Grid::addMargin(std::size_t i, std::size_t j)
{
    assert(isEmptyOrMargin(i, j));
    ++mCells.get(i, j);
}

void Grid::removeMargin(std::size_t i, std::size_t j)
{
    assert(isEmptyOrMargin(i, j)); // The cell may be empty if we just removed the collision box there
    auto& cell = mCells.get(i, j);
    cell = static_cast<int8_t>(std::max(0, cell - 1));
}

void Grid::setFull(std::size_t i, std::size_t j)
{
    assert(mCells.get(i, j) == 0 || mCells.get(i, j) == 1); // The cell may be a margin if we just set the margin box there
    mCells.get(i, j) = static_cast<int8_t>(Grid::Cell::Full);
}

std::array<std::size_t, 4> Grid::getCorners(int i, int j, const Box& box) const
{
    auto yStart = static_cast<std::size_t>(i + box.top);
    auto yEnd = static_cast<std::size_t>(i + box.getBottom());
    auto xStart = static_cast<std::size_t>(j + box.left);
    auto xEnd = static_cast<std::size_t>(j + box.getRight());
    assert((yStart >= yEnd || (yStart < yEnd && yEnd <= getHeight())) &&
        (xStart >= xEnd || (xStart < xEnd && xEnd <= getWidth())));
    return {yStart, yEnd, xStart, xEnd};
}

bool Grid::checkInBox(const std::array<std::size_t, 4>& corners,
    std::invocable<std::size_t, std::size_t> auto&& predicate)
{
    const auto& [yStart, yEnd, xStart, xEnd] = corners;
    for (auto y = yStart; y < yEnd; ++y)
    {
        for (auto x = xStart; x < xEnd; ++x)
        {
            if (!predicate(y, x))
                return false;
        }
    }
    return true;
}

void Grid::applyInBox(const std::array<std::size_t, 4>& corners,
    std::invocable<std::size_t, std::size_t> auto&& function)
{
    const auto& [yStart, yEnd, xStart, xEnd] = corners;
    for (auto y = yStart; y < yEnd; ++y)
    {
        for (auto x = xStart; x < xEnd; ++x)
            function(y, x);
    }
}

Array2<uint32_t>& Grid::getDistancesToWall(Direction direction)
{
    return mDistancesToWall[static_cast<std::size_t>(direction)];
}

const Array2<uint32_t>& Grid::getDistancesToWall(Direction direction) const
{
    return mDistancesToWall[static_cast<std::size_t>(direction)];
}

}

// Explicit instantiations of isDistantToWall
template bool Grid::isDistantToWall<Direction::North>(int i, int j, const Box& box, uint32_t distance) const;
template bool Grid::isDistantToWall<Direction::West>(int i, int j, const Box& box, uint32_t distance) const;
template bool Grid::isDistantToWall<Direction::South>(int i, int j, const Box& box, uint32_t distance) const;
template bool Grid::isDistantToWall<Direction::East>(int i, int j, const Box& box, uint32_t distance) const;