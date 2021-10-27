#pragma once

#ifndef RAZOR_TRANSFORMGROUP_HPP
#define RAZOR_TRANSFORMGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class TransformGroup final : public ComponentGroup {
public:
  TransformGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_TRANSFORMGROUP_HPP
