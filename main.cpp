#include "RaZor/Interface/MainWindow.hpp"

#include <QtWidgets/QtWidgets>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("RaZor");
  QCoreApplication::setApplicationName("RaZor");

  QSplashScreen splashScreen(QPixmap(":/logo/256"));
  splashScreen.show();

  app.thread()->sleep(1);
  app.processEvents();

  MainWindow window;
  window.show();
  window.initializeApplication();

  splashScreen.finish(&window);

  return app.exec();
}
