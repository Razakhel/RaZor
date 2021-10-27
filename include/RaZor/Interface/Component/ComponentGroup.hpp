#pragma once

#ifndef RAZOR_COMPONENTGROUP_HPP
#define RAZOR_COMPONENTGROUP_HPP

#include <QGroupBox>

namespace Raz { class Entity; }

class AppWindow;

class ComponentGroup : public QGroupBox {
public:
  ComponentGroup(Raz::Entity& entity, AppWindow& appWindow);

  virtual void removeComponent() = 0;

protected:
  void resizeEvent(QResizeEvent* event) override;

  Raz::Entity& m_entity;
  class QPushButton* m_optionsButton {};
};

#endif // RAZOR_COMPONENTGROUP_HPP
