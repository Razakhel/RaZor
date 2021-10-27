#pragma once

#ifndef RAZOR_LIGHTGROUP_HPP
#define RAZOR_LIGHTGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class LightGroup final : public ComponentGroup {
public:
  LightGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;

private:
  AppWindow& m_appWindow;
};

#endif // RAZOR_LIGHTGROUP_HPP
