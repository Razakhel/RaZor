#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "ui_ColliderComp.h"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/Collider.hpp>

#include <QStandardItemModel>

void AppWindow::showColliderComponent(Raz::Collider& collider) {
  Ui::ColliderComp colliderComp;

  auto* colliderWidget = new QGroupBox();
  colliderComp.setupUi(colliderWidget);

  // Shape type

  auto* model = static_cast<QStandardItemModel*>(colliderComp.shapeType->model());

  colliderComp.shapeType->addItem(tr("Line"));
  model->item(0)->setEnabled(false);
  colliderComp.shapeType->addItem(tr("Plane"));
  colliderComp.shapeType->addItem(tr("Sphere"));
  model->item(2)->setEnabled(false);
  colliderComp.shapeType->addItem(tr("Triangle"));
  model->item(3)->setEnabled(false);
  colliderComp.shapeType->addItem(tr("Quad"));
  model->item(4)->setEnabled(false);
  colliderComp.shapeType->addItem(tr("AABB"));
  colliderComp.shapeType->addItem(tr("OBB"));
  model->item(6)->setEnabled(false);

  colliderComp.shapeType->setCurrentIndex(static_cast<int>(collider.getShapeType()));

  connect(colliderComp.shapeType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, &collider] (int index) {
    switch (static_cast<Raz::ShapeType>(index)) {
      case Raz::ShapeType::LINE:
        collider.setShape(Raz::Line(Raz::Vec3f(-1.f, 0.f, 0.f), Raz::Vec3f(1.f, 0.f, 0.f)));
        break;

      case Raz::ShapeType::PLANE:
        collider.setShape(Raz::Plane(0.f, Raz::Axis::Y));
        break;

      case Raz::ShapeType::SPHERE:
        collider.setShape(Raz::Sphere(Raz::Vec3f(0.f), 1.f));
        break;

      case Raz::ShapeType::TRIANGLE:
        collider.setShape(Raz::Triangle(Raz::Vec3f(-1.f, -1.f, 0.f), Raz::Vec3f(0.f, 1.f, 0.f), Raz::Vec3f(1.f, -1.f, 0.f)));
        break;

      case Raz::ShapeType::QUAD:
        collider.setShape(Raz::Quad(Raz::Vec3f(-1.f, 1.f, 0.f), Raz::Vec3f(1.f, 1.f, 0.f),
                                    Raz::Vec3f(1.f, -1.f, 0.f), Raz::Vec3f(-1.f, -1.f, 0.f)));
        break;

      case Raz::ShapeType::AABB:
        collider.setShape(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
        break;

      case Raz::ShapeType::OBB:
        collider.setShape(Raz::OBB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
        break;

      default:
        assert("[RaZor] Error: Unhandled collider shape type." && false);
    }

    loadComponents();
  });

  m_parentWindow->m_window.componentsLayout->addWidget(colliderWidget);
}

void AppWindow::showAddCollider(Raz::Entity& entity, const QString& entityName, QMenu& contextMenu) {
  QMenu* addCollider = contextMenu.addMenu(tr("Collider"));

  if (entity.hasComponent<Raz::Collider>() || !entity.hasComponent<Raz::Transform>()) {
    addCollider->setEnabled(false);
    return;
  }

  QAction* addLineCollider = addCollider->addAction(tr("Line"));
  connect(addLineCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::Line(Raz::Vec3f(-1.f, 0.f, 0.f), Raz::Vec3f(1.f, 0.f, 0.f)));
    loadComponents(entityName);
  });
  addLineCollider->setEnabled(false);

  QAction* addPlaneCollider = addCollider->addAction(tr("Plane"));
  connect(addPlaneCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::Plane(0.f, Raz::Axis::Y));
    loadComponents(entityName);
  });

  QAction* addSphereCollider = addCollider->addAction(tr("Sphere"));
  connect(addSphereCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));
    loadComponents(entityName);
  });
  addSphereCollider->setEnabled(false);

  QAction* addTriangleCollider = addCollider->addAction(tr("Triangle"));
  connect(addTriangleCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::Triangle(Raz::Vec3f(-1.f, -1.f, 0.f), Raz::Vec3f(0.f, 1.f, 0.f), Raz::Vec3f(1.f, -1.f, 0.f)));
    loadComponents(entityName);
  });
  addTriangleCollider->setEnabled(false);

  QAction* addQuadCollider = addCollider->addAction(tr("Quad"));
  connect(addQuadCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::Quad(Raz::Vec3f(-1.f, 1.f, 0.f), Raz::Vec3f(1.f, 1.f, 0.f),
                                                 Raz::Vec3f(1.f, -1.f, 0.f), Raz::Vec3f(-1.f, -1.f, 0.f)));
    loadComponents(entityName);
  });
  addQuadCollider->setEnabled(false);

  QAction* addAABBCollider = addCollider->addAction(tr("AABB"));
  connect(addAABBCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
    loadComponents(entityName);
  });

  QAction* addOBBCollider = addCollider->addAction(tr("OBB"));
  connect(addOBBCollider, &QAction::triggered, [this, &entity, entityName] () {
    entity.addComponent<Raz::Collider>(Raz::OBB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
    loadComponents(entityName);
  });
  addOBBCollider->setEnabled(false);
}
