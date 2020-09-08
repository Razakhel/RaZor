#pragma once

#ifndef RAZOR_APPWINDOW_HPP
#define RAZOR_APPWINDOW_HPP

#include <RaZ/Application.hpp>

#include <QWindow>

#include <unordered_map>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QHash>

template <>
struct std::hash<QString> {
  std::size_t operator()(const QString& str) const noexcept { return static_cast<std::size_t>(qHash(str)); }
};
#endif

class MainWindow;

namespace Raz {

class Camera;
class Transform;

} // namespace Raz

class AppWindow : public QWindow {
  friend MainWindow;

public:
  AppWindow();

  void initialize();
  void render();

protected:
  bool event(QEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void exposeEvent(QExposeEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  Raz::Entity& addEntity(QString name);
  void importMesh(const Raz::FilePath& filePath);
  void clearComponents();
  void loadComponents(const QString& entityName);
  void showAddComponent(Raz::Entity& entity, const QString& entityName);
  void processActions();

  MainWindow* m_parentWindow {};

  QOpenGLContext* m_context {};

  Raz::Application m_application {};
  Raz::Camera* m_cameraComp {};
  Raz::Transform* m_cameraTrans {};

  std::unordered_map<QString, Raz::Entity*> m_entities {};

  bool m_movingRight    = false;
  bool m_movingLeft     = false;
  bool m_movingUp       = false;
  bool m_movingDown     = false;
  bool m_movingForward  = false;
  bool m_movingBackward = false;

  bool m_leftClickPressed   = false;
  bool m_middleClickPressed = false;
  bool m_rightClickPressed  = false;

  QPointF m_prevMousePos {};
};

#endif // RAZOR_APPWINDOW_HPP
