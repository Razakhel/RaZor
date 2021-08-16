#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/EntitiesList.hpp"

#include <QAction>

namespace {

class EntityItem final : public QListWidgetItem {
public:
  explicit EntityItem(const QString& name) : QListWidgetItem(name) {
    setFlags(flags() | Qt::ItemFlag::ItemIsUserCheckable);
    setCheckState(Qt::CheckState::Checked);
  }
};

} // namespace

EntitiesList::EntitiesList(QWidget* parent) : QListWidget(parent) {
  setupActions();
}

void EntitiesList::addEntity(const QString& name) {
  auto* entityItem = new EntityItem(name);
  addItem(entityItem);
}

void EntitiesList::setupActions() {
  setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

  auto* removeEntity = new QAction(tr("Remove"), this);
  connect(removeEntity, &QAction::triggered, [this] () {
    // Removing all selected entities
    for (QListWidgetItem* item : selectedItems()) {
      m_appWindow->removeEntity(item->text());
      delete takeItem(row(item));
    }
  });

  addAction(removeEntity);

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

  connect(this, &QListWidget::itemDoubleClicked, [] (QListWidgetItem* item) {
    const bool isChecked = (item->checkState() != Qt::CheckState::Unchecked);
    item->setCheckState((isChecked ? Qt::CheckState::Unchecked : Qt::CheckState::Checked));
  });
}
