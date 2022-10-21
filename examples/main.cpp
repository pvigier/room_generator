#include <iostream>
#include "Solver.h"

int main()
{
    auto rd = std::random_device();
    auto rng = room::RandomGenerator(rd());
    auto variables = std::vector<room::Variable>
    {
        room::Variable
        {
            room::Box{0, 0, 2, 1},
            room::Box{0, 0, 2, 2}
        },
        room::Variable
        {
            room::Box{0, 0, 1, 1},
            room::Box{0, 0, 1, 2}
        },
        room::Variable
        {
            room::Box{0, 0, 3, 3},
            room::Box{0, 0, 3, 3}
        }
    };
    auto optionalVariables = std::vector<room::Variable>();
    auto cells = room::Array2<room::Grid::Cell>(9, 9, room::Grid::Cell::Empty);
    for (auto i = std::size_t(0); i < cells.getHeight(); ++i)
    {
        cells.get(i, 0) = room::Grid::Cell::Wall;
        cells.get(i, cells.getWidth() - 1) = room::Grid::Cell::Wall;
    }
    for (auto j = std::size_t(0); j < cells.getWidth(); ++j)
    {
        cells.get(0, j) = room::Grid::Cell::Wall;
        cells.get(cells.getHeight() - 1, j) = room::Grid::Cell::Wall;
    }
    auto doors = std::vector<room::Grid::Door>();
    auto solver = room::Solver(rng, variables, optionalVariables, cells, doors, true);
    auto result = solver.solve();
    if (!result.has_value())
        std::cout << "No result!\n";
    else
    {
        const auto& newCells = result->cells;
        for (auto i = std::size_t(0); i < newCells.getHeight(); ++i)
        {
            for (auto j = std::size_t(0); j < newCells.getWidth(); ++j)
            {
                switch (newCells.get(i, j))
                {
                    case room::Grid::Cell::Empty:
                        std::cout << ' ';
                        break;
                    case room::Grid::Cell::Margin:
                        std::cout << '.';
                        break;
                    case room::Grid::Cell::Full:
                        std::cout << 'X';
                        break;
                    case room::Grid::Cell::Wall:
                        std::cout << '#';
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            std::cout << '\n';
        }
    }
    return 0;
}