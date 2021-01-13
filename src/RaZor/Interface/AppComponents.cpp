#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "ui_CameraComp.h"
#include "ui_ColliderComp.h"
#include "ui_LightComp.h"
#include "ui_MeshComp.h"
#include "ui_RigidBodyComp.h"
#include "ui_SoundComp.h"
#include "ui_TransformComp.h"

#include <RaZ/Audio/Sound.hpp>
#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/Collider.hpp>
#include <RaZ/Physics/RigidBody.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#include <QStandardItemModel>

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

  if (entity.hasComponent<Raz::RigidBody>()) {
    showRigidBodyComponent(entity.getComponent<Raz::RigidBody>());
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Collider>()) {
    showColliderComponent(entity.getComponent<Raz::Collider>());
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Sound>()) {
    showSoundComponent(entity.getComponent<Raz::Sound>());
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

  connect(transformComp.positionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(static_cast<float>(val), transform.getPosition()[1], transform.getPosition()[2]);

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition()[0], static_cast<float>(val), transform.getPosition()[2]);

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform, &entity, &renderSystem] (double val) {
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

  connect(transformComp.scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
    transform.setScale(static_cast<float>(val), transform.getScale()[1], transform.getScale()[2]);
  });
  connect(transformComp.scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale()[0], static_cast<float>(val), transform.getScale()[2]);
  });
  connect(transformComp.scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
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

  connect(cameraComp.fieldOfView, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&camera] (double val) {
    camera.setFieldOfView(Raz::Degreesd(val));
  });

  // Camera type

  connect(cameraComp.camType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
    camera.setCameraType((index == 0 ? Raz::CameraType::FREE_FLY : Raz::CameraType::LOOK_AT));
  });

  // Projection type

  connect(cameraComp.projType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
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

  connect(meshComp.renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [&mesh] (int index) {
    mesh.setRenderMode((index == 0 ? Raz::RenderMode::TRIANGLE : Raz::RenderMode::POINT));
  });

  // Mesh file

  connect(meshComp.meshFile, &QLineEdit::textChanged, [&mesh, &renderSystem] (const QString& filePath) {
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

  connect(lightComp.directionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(static_cast<float>(val), light.getDirection()[1], light.getDirection()[2]));
    renderSystem.updateLights();
  });
  connect(lightComp.directionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection()[0], static_cast<float>(val), light.getDirection()[2]));
    renderSystem.updateLights();
  });
  connect(lightComp.directionZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection()[0], light.getDirection()[1], static_cast<float>(val)));
    renderSystem.updateLights();
  });

  // Energy

  lightComp.energy->setValue(static_cast<double>(light.getEnergy()));

  connect(lightComp.energy, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light, &renderSystem] (double val) {
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

  connect(lightComp.colorR, &QSlider::valueChanged, updateLightColor);
  connect(lightComp.colorG, &QSlider::valueChanged, updateLightColor);
  connect(lightComp.colorB, &QSlider::valueChanged, updateLightColor);

  m_parentWindow->m_window.componentsLayout->addWidget(lightWidget);
}

void AppWindow::showRigidBodyComponent(Raz::RigidBody& rigidBody) {
  Ui::RigidBodyComp rigidBodyComp;

  auto* rigidBodyWidget = new QGroupBox();
  rigidBodyComp.setupUi(rigidBodyWidget);

  // Mass

  rigidBodyComp.mass->setValue(static_cast<double>(rigidBody.getMass()));

  connect(rigidBodyComp.mass, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&rigidBody] (double val) {
    rigidBody.setMass(static_cast<float>(val));
  });

  // Bounciness

  rigidBodyComp.bounciness->setValue(static_cast<double>(rigidBody.getBounciness()));

  connect(rigidBodyComp.bounciness, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&rigidBody] (double val) {
    rigidBody.setBounciness(static_cast<float>(val));
  });

  // Velocity

  rigidBodyComp.velocityX->setValue(static_cast<double>(rigidBody.getVelocity().x()));
  rigidBodyComp.velocityY->setValue(static_cast<double>(rigidBody.getVelocity().y()));
  rigidBodyComp.velocityZ->setValue(static_cast<double>(rigidBody.getVelocity().z()));

  const auto updateVelocity = [&rigidBody, rigidBodyComp] (double) {
    const auto velocityX = static_cast<float>(rigidBodyComp.velocityX->value());
    const auto velocityY = static_cast<float>(rigidBodyComp.velocityY->value());
    const auto velocityZ = static_cast<float>(rigidBodyComp.velocityZ->value());

    rigidBody.setVelocity(Raz::Vec3f(velocityX, velocityY, velocityZ));
  };

  connect(rigidBodyComp.velocityX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), updateVelocity);

  m_parentWindow->m_window.componentsLayout->addWidget(rigidBodyWidget);
}

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

  connect(colliderComp.shapeType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&collider] (int index) {
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
  });

  // TODO: show a widget corresponding to the collider's shape type to allow modifying it

  m_parentWindow->m_window.componentsLayout->addWidget(colliderWidget);
}

void AppWindow::showSoundComponent(Raz::Sound& sound) {
  Ui::SoundComp soundComp;

  auto* soundWidget = new QGroupBox();
  soundComp.setupUi(soundWidget);

  // Format & frequency

  const auto updateFormat = [&sound, format = soundComp.format] () {
    QString formatStr;

    switch (sound.getFormat()) {
      case Raz::SoundFormat::MONO_U8:
        formatStr = "Mono (8)";
        break;

      case Raz::SoundFormat::MONO_I16:
        formatStr = "Mono (16)";
        break;

      case Raz::SoundFormat::MONO_F32:
        formatStr = "Mono (32)";
        break;

      case Raz::SoundFormat::MONO_F64:
        formatStr = "Mono (64)";
        break;

      case Raz::SoundFormat::STEREO_U8:
        formatStr = "Stereo (8)";
        break;

      case Raz::SoundFormat::STEREO_I16:
        formatStr = "Stereo (16)";
        break;

      case Raz::SoundFormat::STEREO_F32:
        formatStr = "Stereo (32)";
        break;

      case Raz::SoundFormat::STEREO_F64:
        formatStr = "Stereo (64)";
        break;

      default:
        formatStr = "None";
        break;
    }

    format->setText(formatStr);
  };

  updateFormat();
  soundComp.frequency->setText(QString::number(sound.getFrequency()));

  // Repeat

  connect(soundComp.repeat, &QCheckBox::toggled, [&sound] (bool checked) { sound.repeat(checked); });

  // Actions

  connect(soundComp.play, &QPushButton::clicked, [&sound] () { sound.play(); });
  connect(soundComp.pause, &QPushButton::clicked, [&sound] () { sound.pause(); });
  connect(soundComp.stop, &QPushButton::clicked, [&sound] () { sound.stop(); });

  // Volume

  const auto maxVolume = static_cast<float>(soundComp.volume->maximum());
  connect(soundComp.volume, &QSlider::valueChanged, [&sound, maxVolume] (int value) { sound.setGain(static_cast<float>(value) / maxVolume); });

  // Sound file

  connect(soundComp.soundFile, &QLineEdit::textChanged, [&sound, updateFormat, frequency = soundComp.frequency] (const QString& filePath) {
    sound.load(filePath.toStdString());
    updateFormat();
    frequency->setText(QString::number(sound.getFrequency()));
  });

  m_parentWindow->m_window.componentsLayout->addWidget(soundWidget);
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
    connect(addPointLight, &QAction::triggered, [this, &entity, &renderSystem, entityName] () {
      entity.addComponent<Raz::Light>(Raz::LightType::POINT, 1.f);
      renderSystem.updateLights();
      loadComponents(entityName);
    });

    QAction* addDirectionalLight = addLight->addAction(tr("Directional light"));
    connect(addDirectionalLight, &QAction::triggered, [this, &entity, &renderSystem, entityName] () {
      entity.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Z, 1.f);
      renderSystem.updateLights();
      loadComponents(entityName);
    });
  }

  // Rigid body

  QAction* addRigidBody = contextMenu->addAction(tr("Rigid body"));

  if (entity.hasComponent<Raz::RigidBody>() || !entity.hasComponent<Raz::Transform>()) {
    addRigidBody->setEnabled(false);
  } else {
    connect(addRigidBody, &QAction::triggered, [this, &entity, entityName] () {
      entity.addComponent<Raz::RigidBody>(1.f, 0.95f);
      loadComponents(entityName);
    });
  }

  // Collider

  QMenu* addCollider = contextMenu->addMenu(tr("Collider"));

  if (entity.hasComponent<Raz::Collider>() || !entity.hasComponent<Raz::Transform>()) {
    addCollider->setEnabled(false);
  } else {
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

  // Sound

  QAction* addSound = contextMenu->addAction(tr("Sound"));

  if (entity.hasComponent<Raz::Sound>()) {
    addSound->setEnabled(false);
  } else {
    connect(addSound, &QAction::triggered, [this, &entity, entityName] () {
      entity.addComponent<Raz::Sound>();
      loadComponents(entityName);
    });
  }

  connect(addComponent, &QPushButton::clicked, contextMenu, [contextMenu] () { contextMenu->popup(QCursor::pos()); });

  m_parentWindow->m_window.componentsLayout->addWidget(addComponent);
}
