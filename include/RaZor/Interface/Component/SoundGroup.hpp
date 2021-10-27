#pragma once

#ifndef RAZOR_SOUNDGROUP_HPP
#define RAZOR_SOUNDGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class SoundGroup final : public ComponentGroup {
public:
  SoundGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_SOUNDGROUP_HPP
