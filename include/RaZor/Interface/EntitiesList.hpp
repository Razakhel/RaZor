#pragma once

#ifndef RAZOR_ENTITIESLIST_HPP
#define RAZOR_ENTITIESLIST_HPP

#include <QListWidget>

class AppWindow;
class MainWindow;

class EntitiesList final : public QListWidget {
  friend MainWindow;

public:
  explicit EntitiesList(QWidget* parent);

  void addEntity(const QString& name);

private:
  void setupActions();

  AppWindow* m_appWindow {};
};

#endif // RAZOR_ENTITIESLIST_HPP
