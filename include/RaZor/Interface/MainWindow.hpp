#pragma once

#ifndef RAZOR_MAINWINDOW_HPP
#define RAZOR_MAINWINDOW_HPP

#include "RaZor/Interface/AppWindow.hpp"
#include "ui_RaZor.h"
#include "ui_AudioSystemSettings.h"
#include "ui_PhysicsSystemSettings.h"
#include "ui_RenderSystemSettings.h"
#include "ui_About.h"

class MainWindow final : public QMainWindow {
  Q_OBJECT
  friend AppWindow;

public:
  MainWindow();

  void initializeApplication();

  ~MainWindow() override = default;

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  void setupActions();

  Ui::MainWindow m_window {};
  AppWindow m_appWindow {};

  Ui::AudioSystemSettings m_audioSystemSettings {};
  QDialog m_audioSystemSettingsDialog {};

  Ui::RenderSystemSettings m_renderSystemSettings {};
  QDialog m_renderSystemSettingsDialog {};

  Ui::PhysicsSystemSettings m_physicsSystemSettings {};
  QDialog m_physicsSystemSettingsDialog {};

  Ui::About m_about {};
  QDialog m_aboutDialog {};
};

#endif // RAZOR_MAINWINDOW_HPP
