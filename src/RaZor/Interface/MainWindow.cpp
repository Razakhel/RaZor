#include "RaZor/Interface/MainWindow.hpp"

MainWindow::MainWindow() {
  setWindowTitle("RaZor");

  QIcon icon;
  icon.addFile(QString::fromUtf8(":/logo/256"), QSize(), QIcon::Normal, QIcon::Off);
  setWindowIcon(icon);

  resize(800, 600);
  setAcceptDrops(true);

  setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

  // Menu bar
  m_menuBar = new QMenuBar(this);
  m_menuBar->setObjectName(QString::fromUtf8("menuBar"));
  m_menuBar->setGeometry(QRect(0, 0, 800, 21));
  setMenuBar(m_menuBar);

  // Menu element 'File'
  m_fileMenu = new QMenu(m_menuBar);
  m_fileMenu->setObjectName(QString::fromUtf8("menuFile"));
  m_fileMenu->setTitle(tr("File"));

  // Menu actions
  m_importAction = new QAction(m_fileMenu);
  m_importAction->setObjectName(QString::fromUtf8("actionImport"));
  m_importAction->setText(tr("Import..."));
  m_importAction->setToolTip(tr("Import a file"));
  m_importAction->setShortcut(QKeySequence::Open);
  connect(m_importAction, &QAction::triggered, this, &MainWindow::openFile);
  m_fileMenu->addAction(m_importAction);

  m_quitAction = new QAction(m_fileMenu);
  m_quitAction->setObjectName(QString::fromUtf8("actionQuit"));
  m_quitAction->setText(tr("Quit"));
  m_quitAction->setToolTip(tr("Quit RaZor"));
  connect(m_quitAction, &QAction::triggered, this, &QMainWindow::close);
  m_fileMenu->addAction(m_quitAction);

  m_menuBar->addAction(m_fileMenu->menuAction());

  // Status bar
  m_statusBar = new QStatusBar(this);
  m_statusBar->setObjectName(QString::fromUtf8("statusBar"));
  m_statusBar->showMessage(tr("Ready"));
  setStatusBar(m_statusBar);

  QMetaObject::connectSlotsByName(this);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Escape)
    close();
}
