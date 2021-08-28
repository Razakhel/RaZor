#pragma once

#ifndef RAZOR_ENTITIESLIST_HPP
#define RAZOR_ENTITIESLIST_HPP

#include <QListWidget>

class EntitiesList final : public QListWidget {
  friend class MainWindow;

public:
  explicit EntitiesList(QWidget* parent);

  void addEntity(const QString& name);

private:
  void setupActions();

  class AppWindow* m_appWindow {};
};

#endif // RAZOR_ENTITIESLIST_HPP
