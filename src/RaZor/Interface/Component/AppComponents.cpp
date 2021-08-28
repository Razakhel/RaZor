#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "ui_CameraComp.h"
#include "ui_LightComp.h"
#include "ui_ListenerComp.h"
#include "ui_MeshComp.h"
#include "ui_RigidBodyComp.h"
#include "ui_SoundComp.h"
#include "ui_TransformComp.h"

#include <RaZ/Audio/Listener.hpp>
#include <RaZ/Audio/Sound.hpp>
#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/Collider.hpp>
#include <RaZ/Physics/RigidBody.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/RenderSystem.hpp>

void AppWindow::loadComponents() {
  loadComponents(m_parentWindow->m_window.entitiesList->currentItem()->text());
}

void AppWindow::loadComponents(const QString& entityName) {
  // Removing all widgets from the components panel
  clearComponents();

  const auto entityIter = m_entities.find(entityName);

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
    showCameraComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Mesh>()) {
    showMeshComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Light>()) {
    showLightComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::RigidBody>()) {
    showRigidBodyComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Collider>()) {
    showColliderComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Sound>()) {
    showSoundComponent(entity);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Listener>()) {
    showListenerComponent(entity);
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

  auto* transformWidget = new ComponentGroup<Raz::Transform>(entity, *this);
  transformComp.setupUi(transformWidget);

  // Position

  transformComp.positionX->setValue(static_cast<double>(transform.getPosition().x()));
  transformComp.positionY->setValue(static_cast<double>(transform.getPosition().y()));
  transformComp.positionZ->setValue(static_cast<double>(transform.getPosition().z()));

  connect(transformComp.positionX, QOverload<double>::of(&ValuePicker::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(static_cast<float>(val), transform.getPosition().y(), transform.getPosition().z());

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionY, QOverload<double>::of(&ValuePicker::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition().x(), static_cast<float>(val), transform.getPosition().z());

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionZ, QOverload<double>::of(&ValuePicker::valueChanged), [&transform, &entity, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition().x(), transform.getPosition().y(), static_cast<float>(val));

    if (entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });

  // Rotation
  // TODO

  // Scale

  transformComp.scaleX->setValue(static_cast<double>(transform.getScale().x()));
  transformComp.scaleY->setValue(static_cast<double>(transform.getScale().y()));
  transformComp.scaleZ->setValue(static_cast<double>(transform.getScale().z()));

  connect(transformComp.scaleX, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(static_cast<float>(val), transform.getScale().y(), transform.getScale().z());
  });
  connect(transformComp.scaleY, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale().x(), static_cast<float>(val), transform.getScale().z());
  });
  connect(transformComp.scaleZ, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale().x(), transform.getScale().y(), static_cast<float>(val));
  });

  m_parentWindow->m_window.componentsLayout->addWidget(transformWidget);
}

void AppWindow::showCameraComponent(Raz::Entity& entity) {
  Ui::CameraComp cameraComp;

  auto* cameraWidget = new ComponentGroup<Raz::Camera>(entity, *this);
  cameraComp.setupUi(cameraWidget);

  auto& camera = entity.getComponent<Raz::Camera>();

  // Field of view

  cameraComp.fieldOfView->setValue(static_cast<double>(Raz::Degreesf(camera.getFieldOfView()).value));

  connect(cameraComp.fieldOfView, QOverload<double>::of(&ValuePicker::valueChanged), [&camera] (double val) {
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

void AppWindow::showMeshComponent(Raz::Entity& entity) {
  const Raz::RenderSystem& renderSystem = m_application.getWorlds().back().getSystem<Raz::RenderSystem>();

  Ui::MeshComp meshComp;

  auto* meshWidget = new ComponentGroup<Raz::Mesh>(entity, *this);
  meshComp.setupUi(meshWidget);

  auto& mesh = entity.getComponent<Raz::Mesh>();

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

void AppWindow::showLightComponent(Raz::Entity& entity) {
  Ui::LightComp lightComp;

  auto* lightWidget = new ComponentGroup<Raz::Light>(entity, *this);
  lightComp.setupUi(lightWidget);

  auto& light = entity.getComponent<Raz::Light>();
  const Raz::RenderSystem& renderSystem = m_application.getWorlds().back().getSystem<Raz::RenderSystem>();

  // Direction

  lightComp.directionX->setValue(static_cast<double>(light.getDirection().x()));
  lightComp.directionY->setValue(static_cast<double>(light.getDirection().y()));
  lightComp.directionZ->setValue(static_cast<double>(light.getDirection().z()));

  connect(lightComp.directionX, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(static_cast<float>(val), light.getDirection().y(), light.getDirection().z()));
    renderSystem.updateLights();
  });
  connect(lightComp.directionY, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection().x(), static_cast<float>(val), light.getDirection().z()));
    renderSystem.updateLights();
  });
  connect(lightComp.directionZ, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection().x(), light.getDirection().y(), static_cast<float>(val)));
    renderSystem.updateLights();
  });

  // Energy

  lightComp.energy->setValue(static_cast<double>(light.getEnergy()));

  connect(lightComp.energy, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
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

void AppWindow::showRigidBodyComponent(Raz::Entity& entity) {
  Ui::RigidBodyComp rigidBodyComp;

  auto* rigidBodyWidget = new ComponentGroup<Raz::RigidBody>(entity, *this);
  rigidBodyComp.setupUi(rigidBodyWidget);

  auto& rigidBody = entity.getComponent<Raz::RigidBody>();

  // Mass

  rigidBodyComp.mass->setValue(static_cast<double>(rigidBody.getMass()));

  connect(rigidBodyComp.mass, QOverload<double>::of(&ValuePicker::valueChanged), [&rigidBody] (double val) {
    rigidBody.setMass(static_cast<float>(val));
  });

  // Bounciness

  rigidBodyComp.bounciness->setValue(static_cast<double>(rigidBody.getBounciness()));

  connect(rigidBodyComp.bounciness, QOverload<double>::of(&ValuePicker::valueChanged), [&rigidBody] (double val) {
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

  connect(rigidBodyComp.velocityX, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityY, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityZ, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);

  m_parentWindow->m_window.componentsLayout->addWidget(rigidBodyWidget);
}

void AppWindow::showSoundComponent(Raz::Entity& entity) {
  Ui::SoundComp soundComp;

  auto* soundWidget = new ComponentGroup<Raz::Sound>(entity, *this);
  soundComp.setupUi(soundWidget);

  auto& sound = entity.getComponent<Raz::Sound>();

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

void AppWindow::showListenerComponent(Raz::Entity& entity) {
  Ui::ListenerComp listenerComp;

  auto* listenerWidget = new ComponentGroup<Raz::Listener>(entity, *this);
  listenerComp.setupUi(listenerWidget);

  auto& listener = entity.getComponent<Raz::Listener>();

  // Position

  const Raz::Vec3f position = listener.recoverPosition();

  listenerComp.positionX->setValue(static_cast<double>(position.x()));
  listenerComp.positionY->setValue(static_cast<double>(position.y()));
  listenerComp.positionZ->setValue(static_cast<double>(position.z()));

  // Velocity

  const Raz::Vec3f velocity = listener.recoverVelocity();

  listenerComp.velocityX->setValue(static_cast<double>(velocity.x()));
  listenerComp.velocityY->setValue(static_cast<double>(velocity.y()));
  listenerComp.velocityZ->setValue(static_cast<double>(velocity.z()));

  // Orientation

  const Raz::Vec3f forward = listener.recoverForwardOrientation();

  listenerComp.forwardX->setValue(static_cast<double>(forward.x()));
  listenerComp.forwardY->setValue(static_cast<double>(forward.y()));
  listenerComp.forwardZ->setValue(static_cast<double>(forward.z()));

  const Raz::Vec3f up = listener.recoverUpOrientation();

  listenerComp.upX->setValue(static_cast<double>(up.x()));
  listenerComp.upY->setValue(static_cast<double>(up.y()));
  listenerComp.upZ->setValue(static_cast<double>(up.z()));

  m_parentWindow->m_window.componentsLayout->addWidget(listenerWidget);
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

  showAddCollider(entity, entityName, *contextMenu);

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
