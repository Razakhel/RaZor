#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"

#include <RaZ/Audio/AudioSystem.hpp>
#include <RaZ/Audio/Listener.hpp>
#include <RaZ/Audio/Sound.hpp>
#include <RaZ/Math/Transform.hpp>
#include <RaZ/Physics/PhysicsSystem.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/Renderer.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#if defined(RAZOR_COMPILER_MSVC)
struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-
#endif

#include <QComboBox>
#include <QDropEvent>
#include <QMimeData>
#include <QOpenGLContext>

using namespace std::literals;

AppWindow::AppWindow() : m_context{ new QOpenGLContext(this) } {
  setSurfaceType(QWindow::SurfaceType::OpenGLSurface);
}

void AppWindow::initialize() {
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) // QSurfaceFormat::ColorSpace & setColorSpace() are only available with Qt 5.10+
  format.setColorSpace(QSurfaceFormat::ColorSpace::DefaultColorSpace);
#endif
  format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
  format.setDepthBufferSize(24);
  format.setSamples(2);

  m_context->setFormat(format);
  m_context->create();
  m_context->makeCurrent(this);

  // Initializing RaZ's application

  Raz::Renderer::initialize();
  Raz::Renderer::enable(Raz::Capability::DEPTH_TEST);
  Raz::Renderer::enable(Raz::Capability::CULL);

  const QSize windowSize = size();

  Raz::World& world = m_application.addWorld(3);

  // Rendering

  QFile vertexShader(":/shader/vertex");
  if (!vertexShader.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::invalid_argument("[RaZor] Error: Vertex shader '" + vertexShader.fileName().toStdString() + "' unrecognized.");

  QFile fragmentShader(":/shader/cook-torrance");
  if (!fragmentShader.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::invalid_argument("[RaZor] Error: Fragment shader '" + fragmentShader.fileName().toStdString() + "' unrecognized.");

  auto& renderSystem = world.addSystem<Raz::RenderSystem>(windowSize.width(), windowSize.height());
  renderSystem.getGeometryProgram().setShaders(Raz::VertexShader::loadFromSource(vertexShader.readAll().toStdString()),
                                               Raz::FragmentShader::loadFromSource(fragmentShader.readAll().toStdString()));

  m_cameraEntity = &addEntity("Camera");
  m_cameraComp   = &m_cameraEntity->addComponent<Raz::Camera>(windowSize.width(), windowSize.height());
  m_cameraTrans  = &m_cameraEntity->addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));
  m_cameraEntity->addComponent<Raz::Listener>(m_cameraTrans->getPosition(), Raz::Mat3f(m_cameraTrans->computeTransformMatrix()));

  Raz::Entity& light = addEntity("Light");
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, 0.f, 1.f),   // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (RGB)
  light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

  // Copying the resources to files on disk, to be imported into RaZ objects. This is dirty, but is the best way for now
  // Note that QFile::copy() doesn't override anything, thus will do nothing if the files already exist

  if (!QDir().exists("assets/")) {
    if (!QDir().mkdir("assets/"))
      throw std::invalid_argument("[RaZor] Error: Couldn't create the assets folder.");
  }

  QFile::copy(":/mesh/default", "assets/default.obj");
  QFile::copy(":/material/default", "assets/default.mtl");
  QFile::copy(":/texture/default_albedo", "assets/default_albedo.png");
  QFile::copy(":/texture/default_normal", "assets/default_normal.png");
  QFile::copy(":/texture/default_metallic", "assets/default_metallic.png");
  QFile::copy(":/texture/default_roughness", "assets/default_roughness.png");

  Raz::Entity& mesh = addEntity("Ball");
  mesh.addComponent<Raz::Mesh>("assets/default.obj");
  mesh.addComponent<Raz::Transform>();

  // Physics

  auto& physicsSystem = world.addSystem<Raz::PhysicsSystem>();

  m_parentWindow->m_physicsSystemSettings.gravityX->setValue(static_cast<double>(physicsSystem.getGravity().x()));
  m_parentWindow->m_physicsSystemSettings.gravityY->setValue(static_cast<double>(physicsSystem.getGravity().y()));
  m_parentWindow->m_physicsSystemSettings.gravityZ->setValue(static_cast<double>(physicsSystem.getGravity().z()));

  m_parentWindow->m_physicsSystemSettings.friction->setValue(static_cast<double>(physicsSystem.getFriction()));

  connect(m_parentWindow->m_physicsSystemSettings.buttonBox, &QDialogButtonBox::accepted, [this, &physicsSystem] () {
    physicsSystem.setGravity(Raz::Vec3f(static_cast<float>(m_parentWindow->m_physicsSystemSettings.gravityX->value()),
                                        static_cast<float>(m_parentWindow->m_physicsSystemSettings.gravityY->value()),
                                        static_cast<float>(m_parentWindow->m_physicsSystemSettings.gravityZ->value())));

    physicsSystem.setFriction(static_cast<float>(m_parentWindow->m_physicsSystemSettings.friction->value()));
  });

  // Audio

  auto& audioSystem = world.addSystem<Raz::AudioSystem>();

  const std::vector<std::string> audioDevices = Raz::AudioSystem::recoverDevices();

  for (const std::string& device : Raz::AudioSystem::recoverDevices())
    m_parentWindow->m_audioSystemSettings.audioDevices->addItem(device.c_str());

  m_parentWindow->m_audioSystemSettings.audioDevices->setCurrentText(audioSystem.recoverCurrentDevice().c_str());

  connect(m_parentWindow->m_audioSystemSettings.audioDevices, &QComboBox::currentTextChanged, [this, &audioSystem] (const QString& deviceName) {
    audioSystem.openDevice(deviceName.toStdString().c_str());

    // Since the audio device has changed, every Sound object must be recreated
    for (std::pair<const QString, Raz::Entity*>& entityPair : m_entities) {
      Raz::Entity& entity = *entityPair.second;

      if (entity.hasComponent<Raz::Sound>())
        entity.getComponent<Raz::Sound>().init();
    }
  });
}

void AppWindow::render() {
  if (!isExposed())
    return;

  processActions();

  Raz::Renderer::clearColor(0.15f, 0.15f, 0.15f, 1.f);
  Raz::Renderer::clear(Raz::MaskType::COLOR | Raz::MaskType::DEPTH | Raz::MaskType::STENCIL);

  m_application.runOnce();

  // Not making the context current with MSVC in Debug mode results in one "QOpenGLContext::swapBuffers() called without
  //  corresponding makeCurrent()" message per frame
  m_context->makeCurrent(this);
  m_context->swapBuffers(this);
  requestUpdate(); // So that the window is always refreshed
}

void AppWindow::updateLights() const {
  m_application.getWorlds().back().getSystem<Raz::RenderSystem>().updateLights();
}

Raz::Entity& AppWindow::addEntity(QString name) {
  // If an entity with the same name already exists, append a number incremented as long as the name isn't unique
  const QString origName = name;
  std::size_t entityId   = 2;

  while (m_entities.find(name) != m_entities.cend())
    name = origName + QString::number(entityId++);

  m_parentWindow->m_window.entitiesList->addEntity(name);

  Raz::Entity& entity = m_application.getWorlds().back().addEntity();
  m_entities.emplace(std::move(name), &entity);

  return entity;
}

Raz::Entity& AppWindow::recoverEntity(const QString& name) {
  auto entityIter = m_entities.find(name);

  if (entityIter == m_entities.cend() || entityIter->second == nullptr)
    throw std::invalid_argument("[RaZor] Error: Unrecognized entity name '" + name.toStdString() + "'.");

  return *entityIter->second;
}

void AppWindow::enableEntity(const QString& name, bool enabled) {
  Raz::Entity& entity = recoverEntity(name);
  entity.enable(enabled);

  if (entity.hasComponent<Raz::Light>())
    updateLights();
}

bool AppWindow::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::UpdateRequest:
      render();
      return true;

    case QEvent::Drop: {
      auto* dropEvent = static_cast<QDropEvent*>(event);

      const Raz::FilePath filePath = dropEvent->mimeData()->text().remove(0, 8).toStdString(); // The text is prepended with "file:///", which must be removed
      const Raz::FilePath fileExt  = filePath.recoverExtension();

      if (fileExt == "obj" || fileExt == "fbx" || fileExt == "off")
        importMesh(filePath);
      else
        std::cerr << "[RaZor] Error: Unrecognized file extension '" << fileExt << "'" << std::endl;

      return true;
    }

    default:
      return QWindow::event(event);
  }
}

void AppWindow::keyPressEvent(QKeyEvent* event) {
  switch(event->key()) {
    case Qt::Key::Key_D:
      m_movingRight = true;
      break;

    case Qt::Key::Key_Q:
      m_movingLeft = true;
      break;

    case Qt::Key::Key_Space:
      m_movingUp = true;
      break;

    case Qt::Key::Key_V:
      m_movingDown = true;
      break;

    case Qt::Key::Key_Z:
      m_movingForward = true;
      break;

    case Qt::Key::Key_S:
      m_movingBackward = true;
      break;

    default:
      break;
  }

  m_parentWindow->keyPressEvent(event);
  QWindow::keyPressEvent(event);
}

void AppWindow::keyReleaseEvent(QKeyEvent* event) {
  switch(event->key()) {
    case Qt::Key::Key_D:
      m_movingRight = false;
      break;

    case Qt::Key::Key_Q:
      m_movingLeft = false;
      break;

    case Qt::Key::Key_Space:
      m_movingUp = false;
      break;

    case Qt::Key::Key_V:
      m_movingDown = false;
      break;

    case Qt::Key::Key_Z:
      m_movingForward = false;
      break;

    case Qt::Key::Key_S:
      m_movingBackward = false;
      break;

    default:
      break;
  }

  m_parentWindow->keyReleaseEvent(event);
  QWindow::keyReleaseEvent(event);
}

void AppWindow::mousePressEvent(QMouseEvent* event) {
  switch(event->button()) {
    case Qt::MouseButton::LeftButton:
      m_leftClickPressed = true;
      break;

    case Qt::MouseButton::MiddleButton:
      m_middleClickPressed = true;
      break;

    case Qt::MouseButton::RightButton:
      m_rightClickPressed = true;
      break;

    default:
      break;
  }

  m_prevMousePos = event->localPos();
}

void AppWindow::mouseReleaseEvent(QMouseEvent* event) {
  switch(event->button()) {
    case Qt::MouseButton::LeftButton:
      m_leftClickPressed = false;
      break;

    case Qt::MouseButton::MiddleButton:
      m_middleClickPressed = false;
      break;

    case Qt::MouseButton::RightButton:
      m_rightClickPressed = false;
      break;

    default:
      break;
  }
}

void AppWindow::mouseMoveEvent(QMouseEvent* event) {
  const QSize windowSize      = size();
  const QPointF& currMousePos = event->localPos();

  const auto mouseMoveX = static_cast<float>(currMousePos.x() - m_prevMousePos.x()) / static_cast<float>(windowSize.width());
  const auto mouseMoveY = static_cast<float>(currMousePos.y() - m_prevMousePos.y()) / static_cast<float>(windowSize.height());

  if (m_rightClickPressed) {
    if (m_cameraComp->getCameraType() == Raz::CameraType::LOOK_AT) {
      m_cameraTrans->move(-mouseMoveX * 50.f, mouseMoveY * 50.f, 0.f);
    } else {
      const float rotationY = -Raz::Pi<float> * mouseMoveX;
      const float rotationX = -Raz::Pi<float> * mouseMoveY;

      m_cameraTrans->rotate(Raz::Radiansf(rotationX), Raz::Radiansf(rotationY));
    }
  }

  if (m_middleClickPressed)
    m_cameraTrans->move(-mouseMoveX * 10.f, mouseMoveY * 10.f, 0);

  // If the camera has a light & has moved, update all of them
  if (m_cameraEntity->hasComponent<Raz::Light>() && (m_rightClickPressed || m_middleClickPressed))
    updateLights();

  m_prevMousePos = currMousePos;
}

void AppWindow::wheelEvent(QWheelEvent* event) {
  // The offset is divided by 120, which is the most common angle; yOffset is then supposed to be either -1 or 1
  // See: https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
  const auto yOffset  = static_cast<float>(event->angleDelta().y()) / 120.f * m_application.getDeltaTime();
  const float moveVal = 200.f * yOffset;

  m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, moveVal));

  m_cameraComp->setOrthoBoundX(m_cameraComp->getOrthoBoundX() - moveVal / 2);
  m_cameraComp->setOrthoBoundY(m_cameraComp->getOrthoBoundY() - moveVal / 2);

  // If the camera has a non-directional light, update all of them
  if (m_cameraEntity->hasComponent<Raz::Light>() && (m_cameraEntity->getComponent<Raz::Light>().getType() != Raz::LightType::DIRECTIONAL))
    updateLights();

  // If handled, the event must be accepted to not be propagated
  event->accept();
}

void AppWindow::exposeEvent(QExposeEvent*) {
  if (isExposed())
    render();
}

void AppWindow::resizeEvent(QResizeEvent* event) {
  m_application.getWorlds().back().getSystem<Raz::RenderSystem>().resizeViewport(static_cast<unsigned int>(event->size().width()),
                                                                                 static_cast<unsigned int>(event->size().height()));
}

void AppWindow::importMesh(const Raz::FilePath& filePath) {
  m_parentWindow->m_window.statusBar->showMessage(tr("Importing ") + filePath.toUtf8().c_str() + "...");

  try {
    Raz::Entity& mesh = addEntity(filePath.recoverFileName(false).toUtf8().c_str());
    mesh.addComponent<Raz::Mesh>(filePath);
    mesh.addComponent<Raz::Transform>();
  } catch (const std::exception& exception) {
    std::cerr << "Failed to import mesh '" << filePath << "'; reason:\n" << exception.what();
  }

  m_parentWindow->m_window.statusBar->showMessage(tr("Finished importing"), 3000);
}

void AppWindow::loadCubemap(const Raz::FilePath& rightTexturePath, const Raz::FilePath& leftTexturePath,
                            const Raz::FilePath& topTexturePath, const Raz::FilePath& bottomTexturePath,
                            const Raz::FilePath& frontTexturePath, const Raz::FilePath& backTexturePath) {
  m_application.getWorlds().back().getSystem<Raz::RenderSystem>().setCubemap(Raz::Cubemap(rightTexturePath, leftTexturePath,
                                                                                          topTexturePath, bottomTexturePath,
                                                                                          frontTexturePath, backTexturePath));
}

void AppWindow::processActions() {
  const float moveVal = 10.f * m_application.getDeltaTime();

  if (m_movingRight)
    m_cameraTrans->move(Raz::Vec3f(moveVal, 0.f, 0.f));

  if (m_movingLeft)
    m_cameraTrans->move(Raz::Vec3f(-moveVal, 0.f, 0.f));

  if (m_movingUp)
    m_cameraTrans->move(Raz::Vec3f(0.f, moveVal, 0.f));

  if (m_movingDown)
    m_cameraTrans->move(Raz::Vec3f(0.f, -moveVal, 0.f));

  if (m_movingForward) {
    m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, moveVal));

    m_cameraComp->setOrthoBoundX(m_cameraComp->getOrthoBoundX() - moveVal / 2);
    m_cameraComp->setOrthoBoundY(m_cameraComp->getOrthoBoundY() - moveVal / 2);
  }

  if (m_movingBackward) {
    m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, -moveVal));

    m_cameraComp->setOrthoBoundX(m_cameraComp->getOrthoBoundX() + moveVal / 2);
    m_cameraComp->setOrthoBoundY(m_cameraComp->getOrthoBoundY() + moveVal / 2);
  }

  // If the camera entity has moved & possesses a Light component, update all lights
  if ((m_movingRight || m_movingLeft || m_movingUp || m_movingDown || m_movingForward || m_movingBackward) && m_cameraEntity->hasComponent<Raz::Light>())
    updateLights();
}
