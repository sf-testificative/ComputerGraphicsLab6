#include "Projection.h"

#include "Matrix4.h"

#include <cmath>

namespace {
Point3D rotateForView(const Point3D& point)
{
    const Matrix4 rotation = Matrix4::rotationX(0.6) * Matrix4::rotationY(0.7);
    return rotation.transformPoint(point);
}
}

namespace Projection {

ProjectedPoint projectPoint(ProjectionType type, const Point3D& point)
{
    const Point3D viewed = rotateForView(point);
    ProjectedPoint result;

    if (type == ProjectionType::Axonometric) {
        result.x = viewed.x;
        result.y = viewed.y;
        result.depth = -viewed.z;
    } else {
        const double z = 6.0 - viewed.z;

        result.x = viewed.x / z;
        result.y = viewed.y / z;
        result.depth = z;
    }

    result.valid = std::isfinite(result.x) && std::isfinite(result.y) && std::isfinite(result.depth);
    return result;
}

}
