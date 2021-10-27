#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/ComponentGroup.hpp"

#include <QMenu>
#include <QPushButton>
#include <QResizeEvent>

ComponentGroup::ComponentGroup(Raz::Entity& entity, AppWindow& appWindow) : m_entity{ entity }, m_optionsButton{ new QPushButton(this) } {
  m_optionsButton->setIcon(QIcon(":/icon/misc/cog"));
  m_optionsButton->setFlat(true);
  m_optionsButton->setAutoFillBackground(true);
  m_optionsButton->setFixedSize(17, 17);
  m_optionsButton->setIconSize(QSize(13, 13));

  auto* contextMenu = new QMenu(m_optionsButton);

  contextMenu->addAction(tr("Remove component"), [this, &appWindow] () {
    removeComponent();
    appWindow.loadComponents();
  });

  connect(m_optionsButton, &QPushButton::clicked, [contextMenu] () { contextMenu->popup(QCursor::pos()); });
}

void ComponentGroup::resizeEvent(QResizeEvent* event) {
  m_optionsButton->move(size().width() - m_optionsButton->size().width(), -1);

  QGroupBox::resizeEvent(event);
}
