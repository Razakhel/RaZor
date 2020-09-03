#pragma once

#ifndef RAZOR_APPWINDOW_HPP
#define RAZOR_APPWINDOW_HPP

#include <RaZ/Application.hpp>

#include <QWindow>

namespace Raz {

class Camera;
class Transform;

} // namespace Raz

class AppWindow : public QWindow {
  friend class MainWindow;

public:
  AppWindow();

  void initialize();
  void render();

protected:
  bool event(QEvent* event) override;
  void exposeEvent(QExposeEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  QOpenGLContext* m_context {};

  Raz::Application m_application {};
};

#endif // RAZOR_APPWINDOW_HPP
