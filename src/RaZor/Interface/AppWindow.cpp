#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/MainWindow.hpp"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/Renderer.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#if defined(RAZOR_COMPILER_MSVC)
struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-
#endif

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QMimeData>
#include <QOpenGLContext>
#include <QResizeEvent>
#include <QVBoxLayout>

using namespace std::literals;

namespace {

QGroupBox* createComponentGroupBox(const QString& title) {
  auto* groupBox = new QGroupBox(title);
  groupBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Maximum);

  return groupBox;
}

QDoubleSpinBox* createComponentSpinBox(float initVal) {
  auto* spinBox = new QDoubleSpinBox();
  spinBox->setMinimum(std::numeric_limits<double>::lowest());
  spinBox->setMaximum(std::numeric_limits<double>::max());
  spinBox->setDecimals(5);
  spinBox->setMaximumWidth(75);
  spinBox->setValue(static_cast<double>(initVal));

  return spinBox;
}

void showTransformComponent(Raz::Transform& transform, QVBoxLayout& layout) {
  auto* transformGroup = createComponentGroupBox("Transform");

  {
    auto* transformLayout = new QGridLayout();
    transformLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    {
      transformLayout->addWidget(new QLabel("Position"), 0, 0);

      auto* positionLayout = new QHBoxLayout();

      {
        auto* xPos = createComponentSpinBox(transform.getPosition()[0]);
        QObject::connect(xPos, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
          transform.setPosition(static_cast<float>(val), transform.getPosition()[1], transform.getPosition()[2]);
        });
        positionLayout->addWidget(xPos);

        auto* yPos = createComponentSpinBox(transform.getPosition()[1]);
        QObject::connect(yPos, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
          transform.setPosition(transform.getPosition()[0], static_cast<float>(val), transform.getPosition()[2]);
        });
        positionLayout->addWidget(yPos);

        auto* zPos = createComponentSpinBox(transform.getPosition()[2]);
        QObject::connect(zPos, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform] (double val) {
          transform.setPosition(transform.getPosition()[0], transform.getPosition()[1], static_cast<float>(val));
        });
        positionLayout->addWidget(zPos);
      }

      transformLayout->addLayout(positionLayout, 0, 1);
    }

    transformGroup->setLayout(transformLayout);
  }

  layout.addWidget(transformGroup);
}

void showCameraComponent(Raz::Camera& camera, QVBoxLayout& layout) {
  auto* cameraGroup = createComponentGroupBox("Camera");

  {
    auto* cameraLayout = new QGridLayout();
    cameraLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    {
      cameraLayout->addWidget(new QLabel("Field of view"), 0, 0);

      auto* fovLayout = new QHBoxLayout();

      {
        auto* fieldOfView = createComponentSpinBox(Raz::Degreesf(camera.getFieldOfView()).value);
        QObject::connect(fieldOfView, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&camera] (double val) {
          camera.setFieldOfView(Raz::Degreesd(val));
        });
        fovLayout->addWidget(fieldOfView);
      }

      cameraLayout->addLayout(fovLayout, 0, 1);
    }

    cameraLayout->addWidget(new QLabel("Camera type"), 1, 0);

    auto* camType = new QComboBox();
    camType->addItem("Free fly");
    camType->addItem("Look-at");
    QObject::connect(camType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
      camera.setCameraType((index == 0 ? Raz::CameraType::FREE_FLY : Raz::CameraType::LOOK_AT));
    });

    cameraLayout->addWidget(camType, 1, 1);

    cameraGroup->setLayout(cameraLayout);
  }

  layout.addWidget(cameraGroup);
}

void showMeshComponent(Raz::Mesh& mesh, QVBoxLayout& layout) {
  auto* meshGroup = createComponentGroupBox("Mesh");

  {
    auto* meshLayout = new QGridLayout();
    meshLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    {
      meshLayout->addWidget(new QLabel("Vertex count"), 0, 0);
      meshLayout->addWidget(new QLabel(QString::number(mesh.recoverVertexCount())), 0, 1);

      meshLayout->addWidget(new QLabel("Triangle count"), 1, 0);
      meshLayout->addWidget(new QLabel(QString::number(mesh.recoverTriangleCount())), 1, 1);

      meshLayout->addWidget(new QLabel("Render mode"), 2, 0);

      auto* renderMode = new QComboBox();
      renderMode->addItem("Triangles");
      renderMode->addItem("Points");
      renderMode->setCurrentIndex((mesh.getSubmeshes().front().getRenderMode() == Raz::RenderMode::TRIANGLE ? 0 : 1));
      QObject::connect(renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [&mesh] (int index) {
        mesh.setRenderMode((index == 0 ? Raz::RenderMode::TRIANGLE : Raz::RenderMode::POINT));
      });

      meshLayout->addWidget(renderMode, 2, 1);
    }

    meshGroup->setLayout(meshLayout);
  }

  layout.addWidget(meshGroup);
}

void showLightComponent(Raz::Light& light, QVBoxLayout& layout) {
  auto* lightGroup = createComponentGroupBox("Light");

  {
    auto* lightLayout = new QGridLayout();
    lightLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    {
      lightLayout->addWidget(new QLabel("Direction"), 0, 0);

      auto* directionLayout = new QHBoxLayout();

      {
        auto* xDir = createComponentSpinBox(light.getDirection()[0]);
        QObject::connect(xDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light] (double val) {
          light.setDirection(Raz::Vec3f(static_cast<float>(val), light.getDirection()[1], light.getDirection()[2]));
        });
        directionLayout->addWidget(xDir);

        auto* yDir = createComponentSpinBox(light.getDirection()[1]);
        QObject::connect(yDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light] (double val) {
          light.setDirection(Raz::Vec3f(light.getDirection()[0], static_cast<float>(val), light.getDirection()[2]));
        });
        directionLayout->addWidget(yDir);

        auto* zDir = createComponentSpinBox(light.getDirection()[2]);
        QObject::connect(zDir, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&light] (double val) {
          light.setDirection(Raz::Vec3f(light.getDirection()[0], light.getDirection()[1], static_cast<float>(val)));
        });
        directionLayout->addWidget(zDir);
      }

      lightLayout->addLayout(directionLayout, 0, 1);
    }

    lightGroup->setLayout(lightLayout);
  }

  layout.addWidget(lightGroup);
}

} // namespace

AppWindow::AppWindow() : m_context{ new QOpenGLContext(this) } {
  setSurfaceType(QWindow::SurfaceType::OpenGLSurface);
}

void AppWindow::initialize() {
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
  format.setColorSpace(QSurfaceFormat::ColorSpace::DefaultColorSpace);
  format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);

  m_context->setFormat(format);
  m_context->create();
  m_context->makeCurrent(this);

  // Initializing RaZ's application

  Raz::Renderer::initialize();
  Raz::Renderer::enable(Raz::Capability::DEPTH_TEST);
  Raz::Renderer::enable(Raz::Capability::CULL);

  const QSize windowSize = size();

  Raz::World& world = m_application.addWorld(3);
  auto& renderSystem = world.addSystem<Raz::RenderSystem>(windowSize.width(), windowSize.height());
  renderSystem.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + std::string("shaders/common.vert")),
                                               Raz::FragmentShader(RAZ_ROOT + std::string("shaders/cook-torrance.frag")));

  Raz::Entity& camera = addEntity("Camera");
  m_cameraComp        = &camera.addComponent<Raz::Camera>(windowSize.width(), windowSize.height());
  m_cameraTrans       = &camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));

  Raz::Entity& light = addEntity("Light");
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, 0.f, 1.f),   // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (RGB)
  light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

  Raz::Entity& mesh = addEntity("Ball");
  mesh.addComponent<Raz::Mesh>(RAZ_ROOT + "assets/meshes/ball.obj"s);
  mesh.addComponent<Raz::Transform>();
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
  const QSize windowSize     = size();
  const QPointF currMousePos = event->localPos();

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

  m_prevMousePos = currMousePos;
}

void AppWindow::wheelEvent(QWheelEvent* event) {
  // The offset is divided by 120, which is the most common angle; yOffset is then supposed to be either -1 or 1
  // See: https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
  const auto yOffset = static_cast<float>(event->angleDelta().y()) / 120.f * m_application.getDeltaTime();

  m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, -200.f * yOffset));

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

Raz::Entity& AppWindow::addEntity(QString name) {
  // If an entity with the same name already exists, append a number incremented as long as the name isn't unique
  const QString origName = name;
  std::size_t entityId   = 2;

  while (m_entities.find(name) != m_entities.cend())
    name = origName + QString::number(entityId++);

  m_parentWindow->m_window.entitiesList->addItem(name);

  Raz::Entity& entity = m_application.getWorlds().back().addEntity();
  m_entities.emplace(std::move(name), &entity);

  return entity;
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

void AppWindow::clearComponents() {
  while (QLayoutItem* item = m_parentWindow->m_window.componentsLayout->takeAt(0)) {
    QWidget* widget = item->widget();

    if (widget)
      widget->deleteLater();
  }
}

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
    showTransformComponent(entity.getComponent<Raz::Transform>(), *m_parentWindow->m_window.componentsLayout);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Camera>()) {
    showCameraComponent(entity.getComponent<Raz::Camera>(), *m_parentWindow->m_window.componentsLayout);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Mesh>()) {
    showMeshComponent(entity.getComponent<Raz::Mesh>(), *m_parentWindow->m_window.componentsLayout);
    --remainingComponentCount;
  }

  if (entity.hasComponent<Raz::Light>()) {
    showLightComponent(entity.getComponent<Raz::Light>(), *m_parentWindow->m_window.componentsLayout);
    --remainingComponentCount;
  }

  if (remainingComponentCount > 0)
    m_parentWindow->m_window.componentsLayout->addWidget(new QLabel(QString::number(remainingComponentCount) + " component(s) was/were not displayed."));
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

  if (m_movingForward)
    m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, moveVal));

  if (m_movingBackward)
    m_cameraTrans->move(Raz::Vec3f(0.f, 0.f, -moveVal));
}
