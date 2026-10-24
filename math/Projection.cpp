#include "Projection.h"

namespace Projection {

Matrix4 matrix(ProjectionType type, double aspect)
{
    if (type == ProjectionType::Perspective) {
        const Matrix4 view = Matrix4::translation(0.0, 0.0, -6.0);
        return Matrix4::perspective(0.82, aspect, 0.1, 100.0) * view;
    }

    const Matrix4 view = Matrix4::rotationX(0.6154797087)
                         * Matrix4::rotationY(0.7853981634)
                         * Matrix4::translation(0.0, 0.0, -6.0);

    return Matrix4::orthographic(-3.4 * aspect, 3.4 * aspect,
                                 -3.4, 3.4, 0.1, 100.0) * view;
}

} // namespace Projection
