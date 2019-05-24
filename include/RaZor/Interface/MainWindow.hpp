#pragma once

#ifndef RAZOR_MAINWINDOW_HPP
#define RAZOR_MAINWINDOW_HPP

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent* event) override;
};

#endif // RAZOR_MAINWINDOW_HPP
