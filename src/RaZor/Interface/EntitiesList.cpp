#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/EntitiesList.hpp"

EntitiesList::EntitiesList(QWidget* parent) : QListWidget(parent) {
  setupActions();
}

void EntitiesList::addEntity(const QString& name) {
  auto* entityItem = new QListWidgetItem(name);

  entityItem->setFlags(entityItem->flags() | Qt::ItemFlag::ItemIsUserCheckable);
  entityItem->setCheckState(Qt::CheckState::Checked);

  addItem(entityItem);
}

void EntitiesList::setupActions() {
  connect(this, &QListWidget::itemChanged, [this] (QListWidgetItem* item) {
    const bool isChecked = (item->checkState() != Qt::CheckState::Unchecked);

    item->setForeground((isChecked ? Qt::GlobalColor::black : Qt::GlobalColor::gray));
    m_appWindow->enableEntity(item->text(), isChecked);
  });

  connect(this, &QListWidget::itemSelectionChanged, [this] () {
    if (currentItem()->isSelected())
      m_appWindow->loadComponents(currentItem()->text());
    else
      m_appWindow->clearComponents(); // If the selection has been cleared, removing everything from the components panel
  });
}
