#include "Solver.h"
#include <numeric>
#include <queue>
#include "Grid.h"

namespace room
{

Solver::Solver(RandomGenerator& generator, std::vector<Variable> variables,
    std::vector<Variable> optionalVariables, const Array2<Grid::Cell>& cells,
    const std::vector<Grid::Door>& doors, bool connected) :
    mGenerator(generator),
    mVariables(std::move(variables)),
    mOptionalVariables(std::move(optionalVariables)),
    mGrid(cells, doors, connected)
{
    // TODO: use a grid slightly larger to avoid bounds checking
}

std::optional<Solver::Solution> Solver::solve()
{
    computeDomains();
    orderVariables();
    auto solution = Solution();
    solution.positions.resize(mVariables.size());
    auto found = solve(0, solution.positions);
    if (found)
    {
        placeOptionals(solution.optionalPositions);
        solution.cells = mGrid.getCells();
        return solution;
    }
    else
        return std::nullopt;
}

void Solver::computeDomains()
{
    for (auto& variable : mVariables)
        computeDomain(variable);
    for (auto& variable : mOptionalVariables)
        computeDomain(variable);
}

void Solver::computeDomain(Variable& variable)
{
    // Set up domain
    // MAYBE: move this inside Grid and remove coordinates that collides with an initial obstacle
    auto box = variable.collisionBox.getUnion(variable.marginBox);
    box.width = std::max(box.width, 1);
    box.height = std::max(box.height, 1);
    auto iMin = -box.top;
    auto iMax = static_cast<int>(mGrid.getHeight()) - box.getBottom();
    auto jMin = -box.left;
    auto jMax = static_cast<int>(mGrid.getWidth()) - box.getRight();
    auto& values = variable.domain;
    values.reserve((static_cast<std::size_t>((iMax - iMin + 1) * (jMax - jMin + 1))));
    for (auto i = iMin; i <= iMax; ++i)
    {
        for (auto j = jMin; j <= jMax; ++j)
            values.emplace_back(j, i);
    }
    // Apply constraints
    if (variable.constraints != nullptr)
    {
        for (const auto& constraint : *variable.constraints)
            constraint->constrain(mGrid, variable);
    }
    // Shuffle domains
    std::shuffle(std::begin(values), std::end(values), mGenerator);
}

void Solver::orderVariables()
{
    mIndices = std::vector<std::size_t>(mVariables.size());
    for (auto i = std::size_t(0); i < mIndices.size(); ++i)
        mIndices[i] = i;
    // Use the minimum-remaining-values heuristic
    std::sort(std::begin(mIndices), std::end(mIndices),
        [this](auto lhs, auto rhs)
        {
            return mVariables[lhs].domain.size() < mVariables[rhs].domain.size();
        });
}

bool Solver::solve(std::size_t depth, std::vector<Vector2i>& positions)
{
    if (depth >= mIndices.size())
        return true;

    const auto& variable = mVariables[mIndices[depth]];
    for (const auto& value : variable.domain)
    {
        if (mGrid.tryPlace(value.y, value.x, variable))
        {
            positions[mIndices[depth]] = value;
            if (solve(depth + 1, positions))
                return true;
            else
            {
                positions.pop_back();
                mGrid.remove(value.y, value.x, variable);
            }
        }
    }
    return false;
}

void Solver::placeOptionals(std::vector<std::optional<Vector2i>>& positions)
{
    // Sort by priority and shuffle the indices with same priority
    auto ranks = std::vector<std::size_t>(mOptionalVariables.size());
    std::iota(std::begin(ranks), std::end(ranks), std::size_t(0));
    std::shuffle(std::begin(ranks), std::end(ranks), mGenerator);
    auto indices = std::vector<std::size_t>(mOptionalVariables.size());
    std::iota(std::begin(indices), std::end(indices), std::size_t(0));
    std::sort(std::begin(indices), std::end(indices), [this, &ranks](std::size_t i, std::size_t j)
    {
        return mOptionalVariables[i].priority > mOptionalVariables[j].priority ||
            (mOptionalVariables[i].priority == mOptionalVariables[j].priority && ranks[i] < ranks[j]);
    });
    // Try to place objects
    // MAYBE: limit the number of tries
    positions.resize(mOptionalVariables.size());
    for (auto i : indices)
    {
        const auto& variable = mOptionalVariables[i];
        for (const auto& value : variable.domain)
        {
            if (mGrid.tryPlace(value.y, value.x, variable))
            {
                positions[i] = value;
                break;
            }
        }
    }
}

}