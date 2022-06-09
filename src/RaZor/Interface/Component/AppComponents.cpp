#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"
#include "RaZor/Interface/Component/CameraGroup.hpp"
#include "RaZor/Interface/Component/ColliderGroup.hpp"
#include "RaZor/Interface/Component/LightGroup.hpp"
#include "RaZor/Interface/Component/ListenerGroup.hpp"
#include "RaZor/Interface/Component/MeshGroup.hpp"
#include "RaZor/Interface/Component/MeshRendererGroup.hpp"
#include "RaZor/Interface/Component/RigidBodyGroup.hpp"
#include "RaZor/Interface/Component/SoundGroup.hpp"
#include "RaZor/Interface/Component/TransformGroup.hpp"

#include <RaZ/Audio/Listener.hpp>
#include <RaZ/Audio/Sound.hpp>
#include <RaZ/Data/Mesh.hpp>
#include <RaZ/Data/MeshFormat.hpp>
#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/Collider.hpp>
#include <RaZ/Physics/RigidBody.hpp>
#include <RaZ/Render/Camera.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/MeshRenderer.hpp>
#include <RaZ/Render/RenderSystem.hpp>
#include <RaZ/Utils/Logger.hpp>

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
    m_parentWindow->m_window.componentsLayout->addWidget(new TransformGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Camera>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new CameraGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Mesh>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new MeshGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::MeshRenderer>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new MeshRendererGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Light>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new LightGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::RigidBody>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new RigidBodyGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Collider>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new ColliderGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Sound>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new SoundGroup(entity, *this));
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Listener>()) {
    m_parentWindow->m_window.componentsLayout->addWidget(new ListenerGroup(entity, *this));
    --remainingComponentCount;
  }

  if (remainingComponentCount > 0)
    m_parentWindow->m_window.componentsLayout->addWidget(new QLabel(QString::number(remainingComponentCount) + tr(" component(s) not displayed.")));

  showAddComponent(entity, entityName, m_application.getWorlds().back().getSystem<Raz::RenderSystem>());
}

void AppWindow::importMesh(const Raz::FilePath& filePath, Raz::Entity& entity) {
  try {
    auto [meshData, meshRendererData] = Raz::MeshFormat::load(filePath);
    entity.addComponent<Raz::Mesh>(std::move(meshData));
    entity.addComponent<Raz::MeshRenderer>(std::move(meshRendererData));

    if (!entity.hasComponent<Raz::Transform>())
      entity.addComponent<Raz::Transform>();
  } catch (const std::exception& exception) {
    Raz::Logger::error(tr("Failed to import mesh").toStdString() + " '" + filePath + "':\n" + exception.what());
  }
}

void AppWindow::clearComponents() {
  while (QLayoutItem* item = m_parentWindow->m_window.componentsLayout->takeAt(0)) {
    QWidget* widget = item->widget();

    if (widget)
      widget->deleteLater();
  }
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

  // Mesh renderer

  QAction* addMeshRenderer = contextMenu->addAction(tr("Mesh renderer"));

  if (entity.hasComponent<Raz::MeshRenderer>() || !entity.hasComponent<Raz::Transform>() || !entity.hasComponent<Raz::Mesh>()) {
    addMeshRenderer->setEnabled(false);
  } else {
    connect(addMeshRenderer, &QAction::triggered, [this, &entity, entityName] () {
      entity.addComponent<Raz::MeshRenderer>().load(entity.getComponent<Raz::Mesh>());
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
