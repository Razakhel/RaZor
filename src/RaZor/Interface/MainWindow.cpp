#include "RaZor/Interface/MainWindow.hpp"

MainWindow::MainWindow() {
  setWindowTitle("RaZor");

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Normal, QIcon::Off);
  setWindowIcon(icon);

  resize(800, 600);
  setAcceptDrops(true);

  setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

  QMetaObject::connectSlotsByName(this);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Escape)
    close();
}
