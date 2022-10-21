#pragma once

#include <optional>
#include <vector>
#include "Array2.h"
#include "Grid.h"
#include "RandomGenerator.h"
#include "Variable.h"

namespace room
{

class Solver
{
public:
    struct Solution
    {
        std::vector<Vector2i> positions;
        std::vector<std::optional<Vector2i>> optionalPositions;
        Array2<Grid::Cell> cells;
    };

    Solver(RandomGenerator& generator, std::vector<Variable> variables,
        std::vector<Variable> optionalVariables, const Array2<Grid::Cell>& cells,
        const std::vector<Grid::Door>& doors, bool connected);

    std::optional<Solution> solve();

private:
    RandomGenerator& mGenerator;
    std::vector<Variable> mVariables;
    std::vector<Variable> mOptionalVariables;
    Grid mGrid;
    std::vector<std::size_t> mIndices;

    void computeDomains();
    void computeDomain(Variable& variable);
    void orderVariables();
    bool solve(std::size_t depth, std::vector<Vector2i>& positions);
    void placeOptionals(std::vector<std::optional<Vector2i>>& positions);
};

}