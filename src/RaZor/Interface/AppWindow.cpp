#include "RaZor/Interface/AppWindow.hpp"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/Renderer.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#include <QOpenGLContext>
#include <QResizeEvent>

using namespace std::literals;

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

  Raz::World& world = m_application.addWorld(1);
  auto& renderSystem = world.addSystem<Raz::RenderSystem>(1280, 720);
  renderSystem.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + std::string("shaders/common.vert")),
                                               Raz::FragmentShader(RAZ_ROOT + std::string("shaders/cook-torrance.frag")));

  Raz::Entity& camera = world.addEntity();
  m_cameraComp        = &camera.addComponent<Raz::Camera>(1280, 720);
  m_cameraTrans       = &camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));

  Raz::Entity& light = m_application.getWorlds().back().addEntity();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, 0.f, 1.f),   // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (RGB)
  light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

  Raz::Entity& mesh = m_application.getWorlds().back().addEntity();
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

  m_context->swapBuffers(this);
  requestUpdate(); // So that the window is always refreshed
}

bool AppWindow::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::UpdateRequest:
      render();
      return true;

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

  QWindow::keyReleaseEvent(event);
}

void AppWindow::exposeEvent(QExposeEvent*) {
  if (isExposed())
    render();
}

void AppWindow::resizeEvent(QResizeEvent* event) {
  m_application.getWorlds().back().getSystem<Raz::RenderSystem>().resizeViewport(static_cast<unsigned int>(event->size().width()),
                                                                                 static_cast<unsigned int>(event->size().height()));
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
