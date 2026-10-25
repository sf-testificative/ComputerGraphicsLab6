#include "PolyhedronWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
struct DrawFace {
    QPolygonF polygon;
    double depth = 0.0;
    QColor color;
};
}

PolyhedronWidget::PolyhedronWidget(QWidget* parent) : QWidget(parent)
{
    setMinimumSize(280, 280);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);
    setPalette(QPalette(QColor(248, 249, 251)));
}

void PolyhedronWidget::setPolyhedron(const Polyhedron& p)
{
    polyhedron = p;
    update();
}

void PolyhedronWidget::setProjectionType(ProjectionType t)
{
    projectionType = t;
    update();
}

void PolyhedronWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(248, 249, 251));
    if (polyhedron.vertices.isEmpty())
        return;


    double radius = 1.0;
    const Point3D modelCenter = polyhedron.center();
    for (const Point3D& v : polyhedron.vertices) {
        const double dx = v.x - modelCenter.x;
        const double dy = v.y - modelCenter.y;
        const double dz = v.z - modelCenter.z;
        radius = qMax(radius, std::sqrt(dx * dx + dy * dy + dz * dz));
    }
    const double axisLength = qMax(2.5, radius * 1.6);

    QVector<ProjectedPoint> projectedVertices;
    projectedVertices.reserve(polyhedron.vertices.size());
    for (const Point3D& v : polyhedron.vertices)
        projectedVertices.append(Projection::projectPoint(projectionType, v));

    const Point3D origin(0.0, 0.0, 0.0);
    const Point3D axisEnds[3] = {
        Point3D(axisLength, 0.0, 0.0),
        Point3D(0.0, axisLength, 0.0),
        Point3D(0.0, 0.0, axisLength)
    };
    const Point3D axisStarts[3] = {
        Point3D(-axisLength, 0.0, 0.0),
        Point3D(0.0, -axisLength, 0.0),
        Point3D(0.0, 0.0, -axisLength)
    };
    const QColor axisColors[3] = {
        QColor(204, 65, 65), QColor(45, 145, 80), QColor(55, 100, 205)
    };
    const char* axisLabels[3] = {"x", "y", "z"};

    QVector<ProjectedPoint> scenePoints = projectedVertices;
    scenePoints.append(Projection::projectPoint(projectionType, origin));
    for (int i = 0; i < 3; ++i) {
        scenePoints.append(Projection::projectPoint(projectionType, axisStarts[i]));
        scenePoints.append(Projection::projectPoint(projectionType, axisEnds[i]));
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    for (const ProjectedPoint& p : scenePoints) {
        if (!p.valid) continue;
        minX = qMin(minX, p.x);
        maxX = qMax(maxX, p.x);
        minY = qMin(minY, p.y);
        maxY = qMax(maxY, p.y);
    }
    if (minX > maxX || minY > maxY)
        return;

    const double availableWidth = qMax(1, width() - 56);
    const double availableHeight = qMax(1, height() - 56);
    const double sceneWidth = qMax(1e-6, maxX - minX);
    const double sceneHeight = qMax(1e-6, maxY - minY);
    const double scale = qMin(availableWidth / sceneWidth, availableHeight / sceneHeight);
    const QPointF center(width() * 0.5, height() * 0.5);
    const QPointF projectedCenter((minX + maxX) * 0.5, (minY + maxY) * 0.5);

    auto toScreen = [&](const ProjectedPoint& p) -> QPointF {
        return QPointF(center.x() + (p.x - projectedCenter.x()) * scale,
                       center.y() - (p.y - projectedCenter.y()) * scale);
    };

    QVector<QPointF> screenVertices;
    QVector<double> depths;
    screenVertices.reserve(projectedVertices.size());
    depths.reserve(projectedVertices.size());
    for (const ProjectedPoint& p : projectedVertices) {
        screenVertices.append(p.valid ? toScreen(p) : QPointF(-10000.0, -10000.0));
        depths.append(p.valid ? p.depth : 0.0);
    }

    QVector<DrawFace> faces;
    for (const Polygon& face : polyhedron.faces) {
        DrawFace drawFace;
        drawFace.color = face.color;
        bool validFace = !face.indices.isEmpty();
        for (int index : face.indices) {
            if (index < 0 || index >= screenVertices.size() || !projectedVertices[index].valid) {
                validFace = false;
                break;
            }
            drawFace.polygon << screenVertices[index];
            drawFace.depth += depths[index];
        }
        if (validFace) {
            drawFace.depth /= face.indices.size();
            faces.append(drawFace);
        }
    }
    std::sort(faces.begin(), faces.end(), [](const DrawFace& a, const DrawFace& b) {
        return a.depth > b.depth;
    });

    const ProjectedPoint projectedOrigin = Projection::projectPoint(projectionType, origin);
    for (int i = 0; i < 3; ++i) {
        const ProjectedPoint start = Projection::projectPoint(projectionType, axisStarts[i]);
        const ProjectedPoint end = Projection::projectPoint(projectionType, axisEnds[i]);
        if (!start.valid || !end.valid || !projectedOrigin.valid) continue;
        QPen pen(axisColors[i], 1.5, Qt::DashLine);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawLine(toScreen(start), toScreen(end));
        painter.setPen(QPen(axisColors[i], 1.0));
        painter.drawText(toScreen(end) + QPointF(5.0, -5.0), QString::fromLatin1(axisLabels[i]));
    }

    painter.setPen(QPen(QColor(35, 45, 58), 1.4));
    for (const DrawFace& face : faces) {
        painter.setBrush(face.color);
        painter.drawPolygon(face.polygon);
    }
}
