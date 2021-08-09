#include "RaZor/Interface/MainWindow.hpp"

#include <QCommandLineParser>
#include <QSplashScreen>
#include <QThread>
#include <QTranslator>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("RaZor");
  QCoreApplication::setApplicationName("RaZor");

  ///////////////////////
  // Language handling //
  ///////////////////////

  QCommandLineParser parser;
  parser.setApplicationDescription("RaZor - Help");
  parser.addHelpOption();

  QCommandLineOption languageOption({ "l", "lang" }, QCoreApplication::tr("Change the application's language."), "language");
  parser.addOption(languageOption);

  parser.process(app);

  QTranslator translator;

  if (parser.isSet(languageOption)) {
    const QString languageValue = parser.value(languageOption);

    if (languageValue.startsWith("fr")) {
      if (!translator.load("french"))
        std::cerr << "[RaZor] French language not found.\n";
    }
  }

  QCoreApplication::installTranslator(&translator);

  ///////////////////
  // Splash screen //
  ///////////////////

  QSplashScreen splashScreen(QPixmap(":/logo/256"));
  splashScreen.show();

  app.thread()->sleep(1);
  app.processEvents();

  ///////////////////
  // Running RaZor //
  ///////////////////

  MainWindow window;
  window.show();
  window.initializeApplication();

  splashScreen.finish(&window);

  return app.exec();
}
