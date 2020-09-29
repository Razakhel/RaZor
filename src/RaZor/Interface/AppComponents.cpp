#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "ui_CameraComp.h"
#include "ui_LightComp.h"
#include "ui_MeshComp.h"
#include "ui_TransformComp.h"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/RenderSystem.hpp>

void AppWindow::loadComponents(const QString& entityName) {
  // Removing all widgets from the components panel
  clearComponents();

  const auto& entityIter = m_entities.find(entityName);

  if (entityIter == m_entities.cend()) {
    m_parentWindow->statusBar()->showMessage(tr("Failed to find an entity named") + "'" + entityName + "'");
    return;
  }

  Raz::Entity& entity = *entityIter->second;
  std::size_t remainingComponentCount = entity.getEnabledComponents().getEnabledBitCount();

  if (entity.hasComponent<Raz::Transform>()) {
    showTransformComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Camera>()) {
    showCameraComponent(entity.getComponent<Raz::Camera>());
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Mesh>()) {
    showMeshComponent(entity.getComponent<Raz::Mesh>());
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Light>()) {
    showLightComponent(entity.getComponent<Raz::Light>());
    --remainingComponentCount;
  }

  if (remainingComponentCount > 0)
    m_parentWindow->m_window.componentsLayout->addWidget(new QLabel(QString::number(remainingComponentCount) + tr(" component(s) not displayed.")));

  showAddComponent(entity, entityName, m_application.getWorlds().back().getSystem<Raz::RenderSystem>());
}

void AppWindow::clearComponents() {
  while (QLayoutItem* item = m_parentWindow->m_window.componentsLayout->takeAt(0)) {
    QWidget* widget = item->widget();

    if (widget)
      widget->deleteLater();
  }
}

void AppWindow::showTransformComponent(Raz::Entity& entity) {
  assert("Error: The entity must have a Transform component to be shown." && entity.hasComponent<Raz::Transform>());

  auto& transform = entity.getComponent<Raz::Transform>();
  const Raz::RenderSystem& renderSystem = m_application.getWorlds().back().getSystem<Raz::RenderSystem>();

  Ui::TransformComp transformComp;

  auto* transformWidget = new QGroupBox();
  transformComp.setupUi(transformWidget);

  // Position

  transformComp.positionX->setValue(static_cast<double>(transform.getPosition()[0]));
  transformComp.positionY->setValue(static_cast<double>(transform.getPosition()[1]));
  transformComp.positionZ->setValue(static_cast<double>(transform.getPosition()[2]));

  QObject::connect(transformComp.positionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(static_cast<float>(val), transform.getPosition()[1], transform.getPosition()[2]);

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  QObject::connect(transformComp.positionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition()[0], static_cast<float>(val), transform.getPosition()[2]);

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  QObject::connect(transformComp.positionZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition()[0], transform.getPosition()[1], static_cast<float>(val));

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });

  // Rotation
  // TODO

  // Scale

  transformComp.scaleX->setValue(static_cast<double>(transform.getScale()[0]));
  transformComp.scaleY->setValue(static_cast<double>(transform.getScale()[1]));
  transformComp.scaleZ->setValue(static_cast<double>(transform.getScale()[2]));

  QObject::connect(transformComp.scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
    transform.setScale(static_cast<float>(val), transform.getScale()[1], transform.getScale()[2]);
  });
  QObject::connect(transformComp.scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale()[0], static_cast<float>(val), transform.getScale()[2]);
  });
  QObject::connect(transformComp.scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale()[0], transform.getScale()[1], static_cast<float>(val));
  });

  m_parentWindow->m_window.componentsLayout->addWidget(transformWidget);
}

void AppWindow::showCameraComponent(Raz::Camera& camera) {
  Ui::CameraComp cameraComp;

  auto* cameraWidget = new QGroupBox();
  cameraComp.setupUi(cameraWidget);

  // Field of view

  cameraComp.fieldOfView->setValue(static_cast<double>(Raz::Degreesf(camera.getFieldOfView()).value));

  QObject::connect(cameraComp.fieldOfView, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&camera] (double val) {
    camera.setFieldOfView(Raz::Degreesd(val));
  });

  // Camera type

  QObject::connect(cameraComp.camType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
    camera.setCameraType((index == 0 ? Raz::CameraType::FREE_FLY : Raz::CameraType::LOOK_AT));
  });

  // Projection type

  QObject::connect(cameraComp.projType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
    camera.setProjectionType((index == 0 ? Raz::ProjectionType::PERSPECTIVE : Raz::ProjectionType::ORTHOGRAPHIC));
  });

  m_parentWindow->m_window.componentsLayout->addWidget(cameraWidget);
}

void AppWindow::showMeshComponent(Raz::Mesh& mesh) {
  const Raz::RenderSystem& renderSystem = m_application.getWorlds().back().getSystem<Raz::RenderSystem>();

  Ui::MeshComp meshComp;

  auto* meshWidget = new QGroupBox();
  meshComp.setupUi(meshWidget);

  // Vertex count

  meshComp.vertexCount->setText(QString::number(mesh.recoverVertexCount()));

  // Triangle count

  meshComp.triangleCount->setText(QString::number(mesh.recoverTriangleCount()));

  // Render mode

  meshComp.renderMode->setCurrentIndex((mesh.getSubmeshes().front().getRenderMode() == Raz::RenderMode::TRIANGLE ? 0 : 1));

  QObject::connect(meshComp.renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [&mesh] (int index) {
    mesh.setRenderMode((index == 0 ? Raz::RenderMode::TRIANGLE : Raz::RenderMode::POINT));
  });

  // Mesh file

  QObject::connect(meshComp.meshFile, &QLineEdit::textChanged, [&mesh, &renderSystem] (const QString& filePath) {
    mesh.import(filePath.toStdString());
    mesh.load(renderSystem.getGeometryProgram());
  });

  m_parentWindow->m_window.componentsLayout->addWidget(meshWidget);
}

void AppWindow::showLightComponent(Raz::Light& light) {
  const Raz::RenderSystem& renderSystem = m_application.getWorlds().back().getSystem<Raz::RenderSystem>();

  Ui::LightComp lightComp;

  auto* lightWidget = new QGroupBox();
  lightComp.setupUi(lightWidget);

  // Direction

  lightComp.directionX->setValue(static_cast<double>(light.getDirection()[0]));
  lightComp.directionY->setValue(static_cast<double>(light.getDirection()[1]));
  lightComp.directionZ->setValue(static_cast<double>(light.getDirection()[2]));

  QObject::connect(lightComp.directionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(static_cast<float>(val), light.getDirection()[1], light.getDirection()[2]));
    renderSystem.updateLights();
  });
  QObject::connect(lightComp.directionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection()[0], static_cast<float>(val), light.getDirection()[2]));
    renderSystem.updateLights();
  });
  QObject::connect(lightComp.directionZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection()[0], light.getDirection()[1], static_cast<float>(val)));
    renderSystem.updateLights();
  });

  // Energy

  lightComp.energy->setValue(static_cast<double>(light.getEnergy()));

  QObject::connect(lightComp.energy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setEnergy(static_cast<float>(val));
    renderSystem.updateLights();
  });

  // Color

  lightComp.colorR->setValue(static_cast<int>(light.getColor()[0] * static_cast<float>(lightComp.colorR->maximum())));
  lightComp.colorG->setValue(static_cast<int>(light.getColor()[1] * static_cast<float>(lightComp.colorG->maximum())));
  lightComp.colorB->setValue(static_cast<int>(light.getColor()[2] * static_cast<float>(lightComp.colorB->maximum())));

  const auto updateLightColor = [&light, lightComp, &renderSystem] (int) {
    const float colorR = static_cast<float>(lightComp.colorR->value()) / static_cast<float>(lightComp.colorR->maximum());
    const float colorG = static_cast<float>(lightComp.colorG->value()) / static_cast<float>(lightComp.colorG->maximum());
    const float colorB = static_cast<float>(lightComp.colorB->value()) / static_cast<float>(lightComp.colorB->maximum());

    light.setColor(Raz::Vec3f(colorR, colorG, colorB));
    renderSystem.updateLights();
  };

  QObject::connect(lightComp.colorR, &QSlider::valueChanged, updateLightColor);
  QObject::connect(lightComp.colorG, &QSlider::valueChanged, updateLightColor);
  QObject::connect(lightComp.colorB, &QSlider::valueChanged, updateLightColor);

  m_parentWindow->m_window.componentsLayout->addWidget(lightWidget);
}

void AppWindow::showAddComponent(Raz::Entity& entity, const QString& entityName, const Raz::RenderSystem& renderSystem) {
  auto* addComponent = new QPushButton(tr("Add component"));

  auto* contextMenu  = new QMenu(tr("Add component"), addComponent);

  // Transform

  QAction* addTransform = contextMenu->addAction(tr("Transform"));

  if (entity.hasComponent<Raz::Transform>()) {
    addTransform->setEnabled(false);
  } else {
    connect(addTransform, &QAction::triggered, [this, &entity, entityName] () {
      entity.addComponent<Raz::Transform>();
      loadComponents(entityName);
    });
  }

  // Mesh

  QAction* addMesh = contextMenu->addAction(tr("Mesh"));

  if (entity.hasComponent<Raz::Mesh>() || !entity.hasComponent<Raz::Transform>()) {
    addMesh->setEnabled(false);
  } else {
    connect(addMesh, &QAction::triggered, [this, &entity, entityName] () {
      entity.addComponent<Raz::Mesh>();
      loadComponents(entityName);
    });
  }

  // Light

  QMenu* addLight = contextMenu->addMenu(tr("Light"));

  if (entity.hasComponent<Raz::Light>() || !entity.hasComponent<Raz::Transform>()) {
    addLight->setEnabled(false);
  } else {
    QAction* addPointLight = addLight->addAction(tr("Point light"));
    connect(addPointLight, &QAction::triggered, [this, &entity, entityName, &renderSystem] () {
      entity.addComponent<Raz::Light>(Raz::LightType::POINT, 1.f);
      renderSystem.updateLights();
      loadComponents(entityName);
    });

    QAction* addDirectionalLight = addLight->addAction(tr("Directional light"));
    connect(addDirectionalLight, &QAction::triggered, [this, &entity, entityName, &renderSystem] () {
      entity.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Z, 1.f);
      renderSystem.updateLights();
      loadComponents(entityName);
    });
  }

  connect(addComponent, &QPushButton::clicked, contextMenu, [contextMenu] () { contextMenu->popup(QCursor::pos()); });

  m_parentWindow->m_window.componentsLayout->addWidget(addComponent);
}
