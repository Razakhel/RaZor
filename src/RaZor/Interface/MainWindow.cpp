#include "RaZor/Interface/MainWindow.hpp"

#include <QFileDialog>
#include <QKeyEvent>

MainWindow::MainWindow() {
  ////////////////////////
  // Initializing RaZor //
  ////////////////////////

  m_window.setupUi(this);

  QWidget* renderSurface = QWidget::createWindowContainer(&m_appWindow);
  //renderSurface->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  setCentralWidget(renderSurface);

  m_appWindow.m_parentWindow = this;

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
  setWindowIcon(icon);

  setAcceptDrops(true);
  setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
  resize(1280, 720);

  setupActions();

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
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Import a file"), QString(), tr("Mesh") + " (*.obj *.fbx *.off)");

  if (fileName.isEmpty())
    return;

  m_appWindow.importMesh(fileName.toStdString());
}

void MainWindow::setupActions() {
  connect(m_window.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
  connect(m_window.actionQuit, &QAction::triggered, this, &QMainWindow::close);

  connect(m_window.viewWindowEntities, &QAction::triggered, m_window.entitiesPanel, &QDockWidget::show);
  connect(m_window.viewWindowComponents, &QAction::triggered, m_window.componentsPanel, &QDockWidget::show);

  connect(m_window.entitiesList, &QListWidget::itemSelectionChanged, &m_appWindow, [this] () {
    m_appWindow.loadComponents(m_window.entitiesList->currentItem()->text());
  });
}
