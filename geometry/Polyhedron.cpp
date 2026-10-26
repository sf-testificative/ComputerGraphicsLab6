#include "Polyhedron.h"

#include <cmath>

void Polyhedron::applyMatrix(const Matrix4& matrix)
{
    for (Point3D& point : vertices) {
        point = matrix.transformPoint(point);
    }
}

Point3D Polyhedron::center() const
{
    if (vertices.isEmpty()) {
        return {};
    }

    Point3D result;
    for (const Point3D& point : vertices) {
        result.x += point.x;
        result.y += point.y;
        result.z += point.z;
    }

    const double count = vertices.size();
    result.x /= count;
    result.y /= count;
    result.z /= count;
    return result;
}

void Polyhedron::normalizeSize(double targetRadius)
{
    if (vertices.isEmpty()) {
        return;
    }

    const Point3D centerPoint = center();
    double maxRadius = 0.0;

    for (const Point3D& point : vertices) {
        const double dx = point.x - centerPoint.x;
        const double dy = point.y - centerPoint.y;
        const double dz = point.z - centerPoint.z;
        maxRadius = qMax(maxRadius, std::sqrt(dx * dx + dy * dy + dz * dz));
    }

    if (maxRadius > 1e-9) {
        const double scale = targetRadius / maxRadius;
        applyMatrix(Matrix4::translation(-centerPoint.x, -centerPoint.y, -centerPoint.z) * Matrix4::scaling(scale, scale, scale));
    }
}
