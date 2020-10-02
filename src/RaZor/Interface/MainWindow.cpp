#include "RaZor/Interface/MainWindow.hpp"

#include <QFileDialog>
#include <QKeyEvent>

MainWindow::MainWindow() {
  ////////////////////////
  // Initializing RaZor //
  ////////////////////////

  m_window.setupUi(this);
  m_audioSystemSettings.setupUi(&m_audioSystemSettingsDialog);
  m_renderSystemSettings.setupUi(&m_renderSystemSettingsDialog);

  QWidget* renderSurface = QWidget::createWindowContainer(&m_appWindow);
  setCentralWidget(renderSurface);

  m_appWindow.m_parentWindow = this;
  m_window.entitiesList->m_appWindow = &m_appWindow;

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
  setWindowIcon(icon);

  resize(1280, 720);
  setupActions();

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

void MainWindow::openFile() {
  const QString filePath = QFileDialog::getOpenFileName(this, tr("Import a file"), QString(), tr("Mesh") + " (*.obj *.fbx *.off)");

  if (filePath.isEmpty())
    return;

  m_appWindow.importMesh(filePath.toStdString());
}

void MainWindow::setupActions() {
  // File menu

  connect(m_window.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
  connect(m_window.actionQuit, &QAction::triggered, this, &QMainWindow::close);

  // View menu

  connect(m_window.viewWindowEntities, &QAction::triggered, m_window.entitiesPanel, &QDockWidget::show);
  connect(m_window.viewWindowComponents, &QAction::triggered, m_window.componentsPanel, &QDockWidget::show);

  // Tools menu

  connect(m_window.audioSystemSettings, &QAction::triggered, &m_audioSystemSettingsDialog, &QDialog::show);
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
      std::cerr << "Failed to load cubemap; reason:\n" << exception.what();
    }
  });

  // Help menu
  // TODO

  // Entities

  connect(m_window.addEntity, &QPushButton::clicked, [this] () { m_appWindow.addEntity("NewEntity"); });
  connect(m_window.unselectEntity, &QPushButton::clicked, m_window.entitiesList, &QListWidget::clearSelection);
}
