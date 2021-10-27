#pragma once

#ifndef RAZOR_COLLIDERGROUP_HPP
#define RAZOR_COLLIDERGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class ColliderGroup final : public ComponentGroup {
public:
  ColliderGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_COLLIDERGROUP_HPP
