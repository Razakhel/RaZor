#pragma once

#ifndef RAZOR_MESHGROUP_HPP
#define RAZOR_MESHGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class MeshGroup final : public ComponentGroup {
public:
  MeshGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_MESHGROUP_HPP
