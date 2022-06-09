#include "RaZor/Interface/MainWindow.hpp"

#include <RaZ/Utils/FilePath.hpp>
#include <RaZ/Utils/Logger.hpp>

#include <QKeyEvent>

MainWindow::MainWindow() {
  ////////////////////////
  // Initializing RaZor //
  ////////////////////////

  m_window.setupUi(this);
  m_audioSystemSettings.setupUi(&m_audioSystemSettingsDialog);
  m_physicsSystemSettings.setupUi(&m_physicsSystemSettingsDialog);
  m_renderSystemSettings.setupUi(&m_renderSystemSettingsDialog);
  m_about.setupUi(&m_aboutDialog);

  QWidget* renderSurface = QWidget::createWindowContainer(&m_appWindow);
  setCentralWidget(renderSurface);

  m_appWindow.m_parentWindow = this;
  m_window.entitiesList->m_appWindow = &m_appWindow;

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
  setWindowIcon(icon);

  resize(1280, 720);
  setupActions();

  // Clipping docks to specific corners, so that the console is between the entities & components panels
  setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::BottomDockWidgetArea);
  setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

  m_window.componentsLayout->setAlignment(Qt::AlignmentFlag::AlignTop); // To pack all components on the top of the panel
  m_window.statusBar->showMessage(tr("Ready"), 3000);

  QMetaObject::connectSlotsByName(this);
}

void MainWindow::initializeApplication() {
  assert("Error: The main window must be shown for the application to be initialized." && isVisible());

  m_appWindow.initialize();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Escape:
      QMainWindow::close();
      break;
  }
}

void MainWindow::setupActions() {
  // File menu

  connect(m_window.actionQuit, &QAction::triggered, this, &QMainWindow::close);

  // View menu

  m_window.entitiesPanel->toggleViewAction()->setText(tr("&Entities"));
  m_window.viewWindows->addAction(m_window.entitiesPanel->toggleViewAction());
  m_window.componentsPanel->toggleViewAction()->setText(tr("&Components"));
  m_window.viewWindows->addAction(m_window.componentsPanel->toggleViewAction());
  m_window.consolePanel->toggleViewAction()->setText(tr("C&onsole"));
  m_window.viewWindows->addAction(m_window.consolePanel->toggleViewAction());

  // Tools menu

  connect(m_window.audioSystemSettings, &QAction::triggered, &m_audioSystemSettingsDialog, &QDialog::show);
  connect(m_window.physicsSystemSettings, &QAction::triggered, &m_physicsSystemSettingsDialog, &QDialog::show);
  connect(m_window.renderSystemSettings, &QAction::triggered, &m_renderSystemSettingsDialog, &QDialog::show);

  connect(&m_renderSystemSettingsDialog, &QDialog::accepted, [this] () {
    const auto isFieldEmpty = [] (FileWidget<FileType::IMAGE>& fileWidget) -> bool {
      if (fileWidget.text().isEmpty()) {
        fileWidget.setPlaceholderText(tr("Unexpected empty field"));
        return true;
      }

      return false;
    };

    bool rejected = isFieldEmpty(*m_renderSystemSettings.cubemapRight);
    rejected      = isFieldEmpty(*m_renderSystemSettings.cubemapLeft) || rejected;
    rejected      = isFieldEmpty(*m_renderSystemSettings.cubemapTop) || rejected;
    rejected      = isFieldEmpty(*m_renderSystemSettings.cubemapBottom) || rejected;
    rejected      = isFieldEmpty(*m_renderSystemSettings.cubemapFront) || rejected;
    rejected      = isFieldEmpty(*m_renderSystemSettings.cubemapBack) || rejected;

    if (rejected) {
      m_renderSystemSettingsDialog.show();
      return;
    }

    try {
      m_appWindow.loadCubemap(m_renderSystemSettings.cubemapRight->text().toStdString(), m_renderSystemSettings.cubemapLeft->text().toStdString(),
                              m_renderSystemSettings.cubemapTop->text().toStdString(), m_renderSystemSettings.cubemapBottom->text().toStdString(),
                              m_renderSystemSettings.cubemapFront->text().toStdString(), m_renderSystemSettings.cubemapBack->text().toStdString());
    } catch (const std::exception& exception) {
      Raz::Logger::error(tr("Failed to load cubemap: ").toStdString() + exception.what());
    }
  });

  // Help menu
  connect(m_window.actionAbout, &QAction::triggered, &m_aboutDialog, &QDialog::show);

  // Entities

  connect(m_window.addEntity, &QPushButton::clicked, [this] () { m_appWindow.addEntity("NewEntity"); });
  connect(m_window.unselectEntity, &QPushButton::clicked, m_window.entitiesList, &QListWidget::clearSelection);

  // Log console

  Raz::Logger::setLoggingFunction([this] (Raz::LoggingLevel level, const std::string& message) {
    QIcon icon;
    QString log;

    switch (level) {
      case Raz::LoggingLevel::ERROR:
        icon.addFile(":/icon/error/16");
        log += "[Error] ";
        break;

      case Raz::LoggingLevel::WARNING:
        icon.addFile(":/icon/warning/16");
        log += "[Warning] ";
        break;

      case Raz::LoggingLevel::INFO:
        icon.addFile(":/icon/info/16");
        log += "[Info] ";
        break;

      case Raz::LoggingLevel::DEBUG:
        log += "[Debug] ";
        break;

      default:
        log += "[Unknown] ";
        break;
    }

    log += message.c_str();

    // If the last message is the same as this one, don't print it
    if (m_window.console->count() > 0 && m_window.console->item(m_window.console->count() - 1)->text() == log)
      return;

    m_window.console->addItem(new QListWidgetItem(icon, log, m_window.console));
    m_window.console->scrollToBottom();
  });

  m_window.loggingLevel->addItem(tr("None"));
  m_window.loggingLevel->addItem(tr("Error"));
  m_window.loggingLevel->addItem(tr("Warning"));
  m_window.loggingLevel->addItem(tr("Info"));
#if defined(RAZOR_CONFIG_DEBUG)
  m_window.loggingLevel->addItem(tr("Debug"));
#endif
  m_window.loggingLevel->addItem(tr("All"));

  m_window.loggingLevel->setCurrentIndex(static_cast<int>(Raz::LoggingLevel::ERROR));
  Raz::Logger::setLoggingLevel(static_cast<Raz::LoggingLevel>(m_window.loggingLevel->currentIndex()));

  connect(m_window.loggingLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), [] (int index) {
    Raz::Logger::setLoggingLevel(static_cast<Raz::LoggingLevel>(index));
  });

  connect(m_window.clearConsole, &QPushButton::clicked, m_window.console, &QListWidget::clear);
}
