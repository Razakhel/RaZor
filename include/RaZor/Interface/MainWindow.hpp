#pragma once

#ifndef RAZOR_MAINWINDOW_HPP
#define RAZOR_MAINWINDOW_HPP

#include <QtWidgets/QtWidgets>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();

protected:
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void openFile() { qDebug() << "Opening file..."; }

private:
  QMenuBar* m_menuBar;
  QMenu* m_fileMenu;
  QAction* m_importAction;
  QAction* m_quitAction;

  QStatusBar* m_statusBar;
};

#endif // RAZOR_MAINWINDOW_HPP
