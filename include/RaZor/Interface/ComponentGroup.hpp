#pragma once

#ifndef RAZOR_COMPONENTGROUP_HPP
#define RAZOR_COMPONENTGROUP_HPP

#include <QGroupBox>

template <typename CompT>
class ComponentGroup final : public QGroupBox {
public:
  ComponentGroup(Raz::Entity& entity, AppWindow& appWindow);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  class QPushButton* m_removeButton {};
};

#include "RaZor/Interface/ComponentGroup.inl"

#endif // RAZOR_COMPONENTGROUP_HPP
