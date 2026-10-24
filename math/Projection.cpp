#include "Projection.h"

#include "Matrix4.h"

#include <cmath>

namespace {
// Повороты нужны только для выбора удобного ракурса: после них применяется
// одна из двух простых формул из презентации.
Point3D rotateForView(const Point3D& point)
{
    const Matrix4 rotation = Matrix4::rotationX(0.6154797087)
                             * Matrix4::rotationY(0.7853981634);
    return rotation.transformPoint(point);
}
}

namespace Projection {

ProjectedPoint projectPoint(ProjectionType type, const Point3D& point)
{
    const Point3D viewed = rotateForView(point);
    ProjectedPoint result;

    if (type == ProjectionType::Axonometric) {
        // Параллельная проекция из лекции: (x, y, z) -> (x, y).
        result.x = viewed.x;
        result.y = viewed.y;
        // Глубина нужна только для порядка рисования граней; на x и y не влияет.
        result.depth = -viewed.z;
    } else {
        // В лекции: (x, y, z) -> (x / z, y / z).
        // Здесь z — расстояние от камеры до точки. Камера расположена на z = 6,
        // поэтому для повернутой точки расстояние равно 6 - viewed.z.
        const double z = 6.0 - viewed.z;
        if (z <= 1e-9)
            return result; // точка на камере или за ней не проецируется.

        result.x = viewed.x / z;
        result.y = viewed.y / z;
        result.depth = z; // дальние грани рисуются раньше ближних.
    }

    result.valid = std::isfinite(result.x) && std::isfinite(result.y)
                   && std::isfinite(result.depth);
    return result;
}

} // namespace Projection
