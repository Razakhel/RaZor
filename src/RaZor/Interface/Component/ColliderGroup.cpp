#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "RaZor/Interface/Component/ColliderGroup.hpp"
#include "ui_ColliderComp.h"
#include "ui_AABBInfo.h"
#include "ui_PlaneInfo.h"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/Collider.hpp>
#include <RaZ/Utils/Shape.hpp>

#include <QStandardItemModel>

namespace {

QFrame* createPlaneWidget(const Raz::Plane& plane) {
  Ui::PlaneInfo planeComp;

  auto* planeWidget = new QFrame();
  planeComp.setupUi(planeWidget);

  planeComp.distance->setValue(static_cast<double>(plane.getDistance()));

  planeComp.normalX->setValue(static_cast<double>(plane.getNormal().x()));
  planeComp.normalY->setValue(static_cast<double>(plane.getNormal().y()));
  planeComp.normalZ->setValue(static_cast<double>(plane.getNormal().z()));

  return planeWidget;
}

QFrame* createAABBWidget(const Raz::AABB& aabb) {
  Ui::AABBInfo aabbComp;

  auto* aabbWidget = new QFrame();
  aabbComp.setupUi(aabbWidget);

  aabbComp.lowerPointX->setValue(static_cast<double>(aabb.getMinPosition().x()));
  aabbComp.lowerPointY->setValue(static_cast<double>(aabb.getMinPosition().y()));
  aabbComp.lowerPointZ->setValue(static_cast<double>(aabb.getMinPosition().z()));

  aabbComp.upperPointX->setValue(static_cast<double>(aabb.getMaxPosition().x()));
  aabbComp.upperPointY->setValue(static_cast<double>(aabb.getMaxPosition().y()));
  aabbComp.upperPointZ->setValue(static_cast<double>(aabb.getMaxPosition().z()));

  return aabbWidget;
}

void showShapeInfo(Raz::Collider& collider, const Ui::ColliderComp& colliderComp) {
  switch (collider.getShapeType()) {
    case Raz::ShapeType::LINE:
//      colliderComp.shapeInfoLayout->addWidget(createLineWidget(collider.getShape<Raz::Line>()));
      break;

    case Raz::ShapeType::PLANE:
      colliderComp.shapeInfoLayout->addWidget(createPlaneWidget(collider.getShape<Raz::Plane>()));
      break;

    case Raz::ShapeType::SPHERE:
//      colliderComp.shapeInfoLayout->addWidget(createSphereWidget(collider.getShape<Raz::Sphere>()));
      break;

    case Raz::ShapeType::TRIANGLE:
//      colliderComp.shapeInfoLayout->addWidget(createTriangleWidget(collider.getShape<Raz::Triangle>()));
      break;

    case Raz::ShapeType::QUAD:
//      colliderComp.shapeInfoLayout->addWidget(createQuadWidget(collider.getShape<Raz::Quad>()));
      break;

    case Raz::ShapeType::AABB:
      colliderComp.shapeInfoLayout->addWidget(createAABBWidget(collider.getShape<Raz::AABB>()));
      break;

    case Raz::ShapeType::OBB:
//      colliderComp.shapeInfoLayout->addWidget(createOBBWidget(collider.getShape<Raz::OBB>()));
      break;
  }
}

} // namespace

ColliderGroup::ColliderGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::ColliderComp colliderComp {};
  colliderComp.setupUi(this);

  auto& collider = m_entity.getComponent<Raz::Collider>();

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

  connect(colliderComp.shapeType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&collider, &appWindow] (int index) {
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

    appWindow.loadComponents();
  });

  showShapeInfo(collider, colliderComp);
}

void ColliderGroup::removeComponent() {
  m_entity.removeComponent<Raz::Collider>();
}
