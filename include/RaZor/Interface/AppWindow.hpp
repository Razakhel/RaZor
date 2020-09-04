#pragma once

#ifndef RAZOR_APPWINDOW_HPP
#define RAZOR_APPWINDOW_HPP

#include <RaZ/Application.hpp>

#include <QWindow>

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
  void exposeEvent(QExposeEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  Raz::Entity& addEntity(QString name);
  void importMesh(const Raz::FilePath& filePath);
  void loadComponents(const QString& entityName);
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
};

#endif // RAZOR_APPWINDOW_HPP
