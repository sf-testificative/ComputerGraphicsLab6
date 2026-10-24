#pragma once

#include <QColor>
#include <QVector>

class Polygon {
public:
    QVector<int> indices;
    QColor color;

    Polygon() = default;
    explicit Polygon(const QVector<int>& vertexIndices,
                     const QColor& faceColor = QColor(95, 160, 210))
        : indices(vertexIndices), color(faceColor)
    {
    }
};
