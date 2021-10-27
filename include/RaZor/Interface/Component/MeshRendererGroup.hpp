#pragma once

#ifndef RAZOR_MESHRENDERERGROUP_HPP
#define RAZOR_MESHRENDERERGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class MeshRendererGroup final : public ComponentGroup {
public:
  MeshRendererGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_MESHRENDERERGROUP_HPP
