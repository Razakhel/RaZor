#pragma once

#ifndef RAZOR_MAINWINDOW_HPP
#define RAZOR_MAINWINDOW_HPP

#include "ui_RaZor.h"
#include "RaZor/Interface/AppWindow.hpp"

class MainWindow : public QMainWindow {
  Q_OBJECT
  friend AppWindow;

public:
  MainWindow();

  void initializeApplication();

  ~MainWindow() override = default;

protected:
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void openFile();

private:
  void setupActions();

  Ui::MainWindow m_window {};
  AppWindow m_renderSurface {};
};

#endif // RAZOR_MAINWINDOW_HPP
