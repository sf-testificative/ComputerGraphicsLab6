#include "MainWindow.h"
#include "Transformations.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

#include <cmath>

namespace {

constexpr double kPi = 3.14159265358979323846;

QDoubleSpinBox* numberBox(double value = 0.0,
                          double minimum = -1000.0,
                          double maximum = 1000.0,
                          int decimals = 2)
{
    auto* spinBox = new QDoubleSpinBox;
    spinBox->setRange(minimum, maximum);
    spinBox->setDecimals(decimals);
    spinBox->setSingleStep(0.1);
    spinBox->setValue(value);
    return spinBox;
}

QPushButton* actionButton(const QString& text)
{
    auto* button = new QPushButton(text);
    button->setMinimumHeight(29);
    return button;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      view(new PolyhedronWidget),
      current(PolyhedronFactory::create(PolyhedronFactory::Hexahedron))
{
    setWindowTitle("Правильные многогранники — 3D преобразования");
    resize(1200, 760);
    setMinimumSize(900, 600);

    auto* central = new QWidget;
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(8);

    auto* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(290);
    scrollArea->setMaximumWidth(360);

    auto* controls = new QWidget;
    auto* controlsLayout = new QVBoxLayout(controls);

    // Model selection and reset.
    shapeBox = new QComboBox;
    for (int i = 0; i < 5; ++i) {
        shapeBox->addItem(PolyhedronFactory::name(
            static_cast<PolyhedronFactory::Type>(i)));
    }

    auto* shapeGroup = new QGroupBox("Модель");
    auto* shapeLayout = new QFormLayout(shapeGroup);
    shapeLayout->addRow("Многогранник:", shapeBox);

    auto* resetButton = actionButton("Сбросить преобразования");
    shapeLayout->addRow(resetButton);
    controlsLayout->addWidget(shapeGroup);

    // Projection selection.
    projectionBox = new QComboBox;
    projectionBox->addItems({"Аксонометрическая", "Перспективная"});

    auto* projectionGroup = new QGroupBox("Проекция");
    auto* projectionLayout = new QFormLayout(projectionGroup);
    projectionLayout->addRow("Тип:", projectionBox);
    controlsLayout->addWidget(projectionGroup);

    // Parameters used by transformations.
    xBox = numberBox();
    yBox = numberBox();
    zBox = numberBox();
    angleBox = numberBox(30.0, -360.0, 360.0);
    scaleBox = numberBox(1.2, 0.05, 10.0);

    auto* transformationGroup = new QGroupBox("Параметры преобразований");
    auto* transformationLayout = new QFormLayout(transformationGroup);
    transformationLayout->addRow("X:", xBox);
    transformationLayout->addRow("Y:", yBox);
    transformationLayout->addRow("Z:", zBox);
    transformationLayout->addRow("Угол (градусы):", angleBox);
    transformationLayout->addRow("Коэффициент масштаба:", scaleBox);
    controlsLayout->addWidget(transformationGroup);

    auto* moveButton = actionButton("Сместить по X / Y / Z");
    controlsLayout->addWidget(moveButton);

    auto* rotationRow = new QHBoxLayout;
    auto* rotateXButton = actionButton("Поворот X");
    auto* rotateYButton = actionButton("Поворот Y");
    auto* rotateZButton = actionButton("Поворот Z");
    rotationRow->addWidget(rotateXButton);
    rotationRow->addWidget(rotateYButton);
    rotationRow->addWidget(rotateZButton);
    controlsLayout->addLayout(rotationRow);

    auto* scaleButton = actionButton("Масштаб относительно центра");
    controlsLayout->addWidget(scaleButton);

    // Reflection across a coordinate plane.
    planeBox = new QComboBox;
    planeBox->addItems({"XY", "XZ", "YZ"});
    auto* reflectButton = actionButton("Отразить относительно плоскости");

    auto* reflectionGroup = new QGroupBox("Отражение");
    auto* reflectionLayout = new QHBoxLayout(reflectionGroup);
    reflectionLayout->addWidget(planeBox);
    reflectionLayout->addWidget(reflectButton);
    controlsLayout->addWidget(reflectionGroup);

    // Rotation around an axis through the polyhedron center.
    axisBox = new QComboBox;
    axisBox->addItems({"X", "Y", "Z"});
    auto* centerRotateButton = actionButton("Вращать вокруг центральной оси");

    auto* axisGroup = new QGroupBox("Вращение через центр");
    auto* axisLayout = new QHBoxLayout(axisGroup);
    axisLayout->addWidget(axisBox);
    axisLayout->addWidget(centerRotateButton);
    controlsLayout->addWidget(axisGroup);

    // Rotation around an arbitrary line through points A and B.
    axBox = numberBox(-1.0);
    ayBox = numberBox();
    azBox = numberBox();
    bxBox = numberBox(1.0);
    byBox = numberBox();
    bzBox = numberBox();

    auto* lineGroup = new QGroupBox("Произвольная прямая через точки A и B");
    auto* lineLayout = new QFormLayout(lineGroup);

    auto* aRow = new QWidget;
    auto* aLayout = new QHBoxLayout(aRow);
    aLayout->setContentsMargins(0, 0, 0, 0);
    aLayout->addWidget(axBox);
    aLayout->addWidget(ayBox);
    aLayout->addWidget(azBox);
    lineLayout->addRow("A (x,y,z):", aRow);

    auto* bRow = new QWidget;
    auto* bLayout = new QHBoxLayout(bRow);
    bLayout->setContentsMargins(0, 0, 0, 0);
    bLayout->addWidget(bxBox);
    bLayout->addWidget(byBox);
    bLayout->addWidget(bzBox);
    lineLayout->addRow("B (x,y,z):", bRow);

    auto* arbitraryRotateButton = actionButton("Повернуть вокруг прямой");
    lineLayout->addRow(arbitraryRotateButton);
    controlsLayout->addWidget(lineGroup);
    controlsLayout->addStretch(1);

    scrollArea->setWidget(controls);
    rootLayout->addWidget(scrollArea);
    rootLayout->addWidget(view, 1);
    setCentralWidget(central);

    view->setPolyhedron(current);

    connect(shapeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                loadShape(index);
            });

    connect(projectionBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                const ProjectionType type = index == 0
                    ? ProjectionType::Axonometric
                    : ProjectionType::Perspective;
                view->setProjectionType(type);
            });

    connect(resetButton, &QPushButton::clicked, this, [this]() {
        loadShape(shapeBox->currentIndex());
    });

    connect(moveButton, &QPushButton::clicked, this, [this]() {
        apply(Matrix4::translation(xBox->value(), yBox->value(), zBox->value()));
    });

    const auto angle = [this]() {
        return angleBox->value() * kPi / 180.0;
    };

    connect(rotateXButton, &QPushButton::clicked, this, [this, angle]() {
        apply(Matrix4::rotationX(angle()));
    });
    connect(rotateYButton, &QPushButton::clicked, this, [this, angle]() {
        apply(Matrix4::rotationY(angle()));
    });
    connect(rotateZButton, &QPushButton::clicked, this, [this, angle]() {
        apply(Matrix4::rotationZ(angle()));
    });

    connect(scaleButton, &QPushButton::clicked, this, [this]() {
        apply(Transformations::scaleAroundCenter(current.center(), scaleBox->value()));
    });

    connect(reflectButton, &QPushButton::clicked, this, [this]() {
        const int index = planeBox->currentIndex();
        const Matrix4 reflection = index == 0
            ? Matrix4::reflectionXY()
            : (index == 1 ? Matrix4::reflectionXZ() : Matrix4::reflectionYZ());
        apply(reflection);
    });

    connect(centerRotateButton, &QPushButton::clicked, this, [this, angle]() {
        apply(Transformations::rotateAroundCenterAxis(
            current.center(), axisBox->currentIndex(), angle()));
    });

    connect(arbitraryRotateButton, &QPushButton::clicked, this, [this, angle]() {
        const Point3D a(axBox->value(), ayBox->value(), azBox->value());
        const Point3D b(bxBox->value(), byBox->value(), bzBox->value());

        if (std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z) < 1e-9) {
            QMessageBox::warning(this, "Неверная прямая",
                                 "Точки A и B должны различаться.");
            return;
        }

        apply(Transformations::rotateAroundLine(a, b, angle()));
    });
}

void MainWindow::loadShape(int index)
{
    current = PolyhedronFactory::create(
        static_cast<PolyhedronFactory::Type>(index));
    view->setPolyhedron(current);
}

void MainWindow::apply(const Matrix4& matrix)
{
    current.applyMatrix(matrix);
    view->setPolyhedron(current);
}
