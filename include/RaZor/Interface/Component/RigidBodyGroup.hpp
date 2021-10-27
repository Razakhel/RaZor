#pragma once

#ifndef RAZOR_RIGIDBODYGROUP_HPP
#define RAZOR_RIGIDBODYGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class RigidBodyGroup final : public ComponentGroup {
public:
  RigidBodyGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_RIGIDBODYGROUP_HPP
