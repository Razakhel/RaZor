#include "RaZor/Interface/MainWindow.hpp"

#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/Mesh.hpp>
#include <RaZ/Render/Renderer.hpp>
#include <RaZ/Render/RenderSystem.hpp>

using namespace Raz::Literals;

MainWindow::MainWindow() {
  ////////////////////////
  // Initializing RaZor //
  ////////////////////////

  m_window.setupUi(this);

  QWidget* renderSurface = QWidget::createWindowContainer(&m_renderSurface);
  //renderSurface->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  setCentralWidget(renderSurface);

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Normal, QIcon::Off);
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

  m_renderSurface.initialize();
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

  m_window.statusBar->showMessage(tr("Importing ") + fileName + "...");

  try {
    Raz::Entity& meshEntity = m_renderSurface.m_application.getWorlds().back().addEntityWithComponent<Raz::Mesh>(fileName.toStdString());

    auto& meshTrans = meshEntity.addComponent<Raz::Transform>();
    meshTrans.scale(0.2f);
    meshTrans.rotate(180.0_deg, Raz::Axis::Y);
  } catch (const std::exception& exception) {
    qDebug() << "Failed to import mesh " << fileName << ": " << exception.what();
  }

  m_window.statusBar->showMessage(tr("Finished importing"), 3000);
}

void MainWindow::setupActions() {
  connect(m_window.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
  connect(m_window.actionQuit, &QAction::triggered, this, &QMainWindow::close);
}
