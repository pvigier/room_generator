#pragma once

#include <array>
#include <concepts>
#include <optional>
#include "Array2.h"
#include "Box.h"
#include "Direction.h"
#include "Vector2.h"

namespace room
{

struct Variable;

class Grid
{
public:
    enum class Cell : int8_t
    {
        Empty = 0,
        Margin = 1,
        Full = -1,
        Wall = -2
    };

    struct Door
    {
        Vector2i position;
    };

    Grid(const Array2<Cell>& cells, const std::vector<Grid::Door>& doors, bool connected);

    std::size_t getHeight() const;
    std::size_t getWidth() const;
    bool isDoor(int i, int j) const;
    template<Direction Direction>
    bool isDistantToWall(int i, int j, const Box& box, uint32_t distance) const;
    bool isInCorner(int i, int j, const Box& box) const;

    bool tryPlace(int i, int j, const Variable& variable);
    void remove(int i, int j, const Variable& variable);

    bool isConnected() const;

    Array2<Cell> getCells() const;

private:
    Array2<std::underlying_type_t<Cell>> mCells;
    Array2<bool> mDoors;
    std::array<Array2<uint32_t>, DirectionCount> mDistancesToWall;
    bool mConnected = true;

    bool mayDisconnect(int iStart, int iEnd, int jStart, int jEnd) const;

    bool isEmpty(std::size_t i, std::size_t j) const;
    bool isEmptyOrMargin(std::size_t i, std::size_t j) const;
    bool isFull(std::size_t i, std::size_t j) const;
    bool isWall(std::size_t i, std::size_t j) const;
    void setEmpty(std::size_t i, std::size_t j);
    void addMargin(std::size_t i, std::size_t j);
    void removeMargin(std::size_t i, std::size_t j);
    void setFull(std::size_t i, std::size_t j);
    std::array<std::size_t, 4> getCorners(int i, int j, const Box& box) const;
    bool checkInBox(const std::array<std::size_t, 4>& corners,
        std::invocable<std::size_t, std::size_t> auto&& predicate);
    void applyInBox(const std::array<std::size_t, 4>& corners,
        std::invocable<std::size_t, std::size_t> auto&& function);

    Array2<uint32_t>& getDistancesToWall(Direction direction);
    const Array2<uint32_t>& getDistancesToWall(Direction direction) const;
};

}