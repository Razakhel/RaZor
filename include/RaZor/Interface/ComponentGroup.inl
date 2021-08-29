#include <RaZ/Entity.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#include <QMenu>
#include <QPushButton>
#include <QResizeEvent>

template <typename CompT>
ComponentGroup<CompT>::ComponentGroup(Raz::Entity& entity, AppWindow& appWindow) : m_removeButton{ new QPushButton(this) } {
  m_removeButton->setIcon(QIcon(":/icon/misc/cog"));
  m_removeButton->setFlat(true);
  m_removeButton->setAutoFillBackground(true);
  m_removeButton->setFixedSize(17, 17);
  m_removeButton->setIconSize(QSize(13, 13));

  // Transform, Camera & Listener components should not be removed; until more options may be available, disabling the button
  if constexpr (std::is_same_v<CompT, Raz::Transform> || std::is_same_v<CompT, Raz::Camera> || std::is_same_v<CompT, Raz::Listener>) {
    m_removeButton->setDisabled(true);
    return;
  }

  auto* contextMenu = new QMenu(m_removeButton);

  contextMenu->addAction(tr("Remove component"), [&entity, &appWindow] () {
    entity.removeComponent<CompT>();

    // If a light has been removed, the RenderSystem has to update the lighting
    if constexpr (std::is_same_v<CompT, Raz::Light>)
      appWindow.updateLights();

    appWindow.loadComponents();
  });

  connect(m_removeButton, &QPushButton::clicked, [this, contextMenu] () { contextMenu->popup(QCursor::pos()); });
}

template <typename CompT>
void ComponentGroup<CompT>::resizeEvent(QResizeEvent* event) {
  m_removeButton->move(size().width() - m_removeButton->size().width(), -1);

  QGroupBox::resizeEvent(event);
}
