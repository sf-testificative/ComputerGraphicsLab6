#pragma once

#include "Matrix4.h"

enum class ProjectionType {
    Perspective,
    Axonometric
};

namespace Projection {

Matrix4 matrix(ProjectionType type, double aspect);

} // namespace Projection
