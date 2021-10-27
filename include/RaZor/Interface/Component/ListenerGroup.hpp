#pragma once

#ifndef RAZOR_LISTENERGROUP_HPP
#define RAZOR_LISTENERGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class ListenerGroup final : public ComponentGroup {
public:
  ListenerGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_LISTENERGROUP_HPP
