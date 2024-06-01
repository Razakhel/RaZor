#pragma once

#ifndef RAZOR_LUASCRIPTGROUP_HPP
#define RAZOR_LUASCRIPTGROUP_HPP

#include "RaZor/Interface/Component/ComponentGroup.hpp"

class LuaScriptGroup final : public ComponentGroup {
public:
  LuaScriptGroup(Raz::Entity& entity, AppWindow& appWindow);

  void removeComponent() override;
};

#endif // RAZOR_LUASCRIPTGROUP_HPP
