#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "Array2.h"
#include "Box.h"
#include "Constraint.h"
#include "Domain.h"

namespace room
{

struct Constraint;

struct Variable
{
    Box collisionBox;
    Box marginBox;
    const std::vector<std::unique_ptr<Constraint>>* constraints = nullptr;
    Domain domain = {};
    int priority = 0;
};

}