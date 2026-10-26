#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../geometry/PolyhedronFactory.h"
#include "../render/PolyhedronWidget.h"

class QComboBox;
class QDoubleSpinBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    PolyhedronWidget* view;
    Polyhedron current;

    QComboBox* shapeBox;
    QComboBox* projectionBox;
    QComboBox* axisBox;
    QComboBox* planeBox;

    QDoubleSpinBox* xBox;
    QDoubleSpinBox* yBox;
    QDoubleSpinBox* zBox;
    QDoubleSpinBox* angleBox;
    QDoubleSpinBox* scaleBox;

    QDoubleSpinBox* axBox;
    QDoubleSpinBox* ayBox;
    QDoubleSpinBox* azBox;
    QDoubleSpinBox* bxBox;
    QDoubleSpinBox* byBox;
    QDoubleSpinBox* bzBox;

    void loadShape(int index);
    void apply(const Matrix4& matrix);
};

#endif