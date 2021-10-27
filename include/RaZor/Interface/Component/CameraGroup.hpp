#pragma once

#ifndef RAZOR_CAMERAGROUP_HPP
#define RAZOR_CAMERAGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class CameraGroup final : public ComponentGroup {
public:
  CameraGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_CAMERAGROUP_HPP
