#ifndef POLYHEDRON_WIDGET_H
#define POLYHEDRON_WIDGET_H

#include <QWidget>
#include "../geometry/Polyhedron.h"
#include "../math/Projection.h"

class PolyhedronWidget : public QWidget {
    Q_OBJECT
public:
    explicit PolyhedronWidget(QWidget* parent = nullptr);
    void setPolyhedron(const Polyhedron& p);
    void setProjectionType(ProjectionType type);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    Polyhedron polyhedron;
    ProjectionType projectionType = ProjectionType::Axonometric;
};

#endif