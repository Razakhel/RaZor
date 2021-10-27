#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/LightGroup.hpp"
#include "ui_LightComp.h"

#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/RenderSystem.hpp>

LightGroup::LightGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow), m_appWindow{ appWindow } {
  Ui::LightComp lightComp {};
  lightComp.setupUi(this);

  auto& light = m_entity.getComponent<Raz::Light>();
  const Raz::RenderSystem& renderSystem = appWindow.getApplication().getWorlds().back().getSystem<Raz::RenderSystem>();

  // Direction

  lightComp.directionX->setValue(static_cast<double>(light.getDirection().x()));
  lightComp.directionY->setValue(static_cast<double>(light.getDirection().y()));
  lightComp.directionZ->setValue(static_cast<double>(light.getDirection().z()));

  connect(lightComp.directionX, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(static_cast<float>(val), light.getDirection().y(), light.getDirection().z()));
    renderSystem.updateLights();
  });
  connect(lightComp.directionY, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection().x(), static_cast<float>(val), light.getDirection().z()));
    renderSystem.updateLights();
  });
  connect(lightComp.directionZ, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setDirection(Raz::Vec3f(light.getDirection().x(), light.getDirection().y(), static_cast<float>(val)));
    renderSystem.updateLights();
  });

  // Energy

  lightComp.energy->setValue(static_cast<double>(light.getEnergy()));

  connect(lightComp.energy, QOverload<double>::of(&ValuePicker::valueChanged), [&light, &renderSystem] (double val) {
    light.setEnergy(static_cast<float>(val));
    renderSystem.updateLights();
  });

  // Color

  lightComp.colorR->setValue(static_cast<int>(light.getColor()[0] * static_cast<float>(lightComp.colorR->maximum())));
  lightComp.colorG->setValue(static_cast<int>(light.getColor()[1] * static_cast<float>(lightComp.colorG->maximum())));
  lightComp.colorB->setValue(static_cast<int>(light.getColor()[2] * static_cast<float>(lightComp.colorB->maximum())));

  const auto updateLightColor = [lightComp, &light, &renderSystem] (int) {
    const float colorR = static_cast<float>(lightComp.colorR->value()) / static_cast<float>(lightComp.colorR->maximum());
    const float colorG = static_cast<float>(lightComp.colorG->value()) / static_cast<float>(lightComp.colorG->maximum());
    const float colorB = static_cast<float>(lightComp.colorB->value()) / static_cast<float>(lightComp.colorB->maximum());

    light.setColor(Raz::Vec3f(colorR, colorG, colorB));
    renderSystem.updateLights();
  };

  connect(lightComp.colorR, &QSlider::valueChanged, updateLightColor);
  connect(lightComp.colorG, &QSlider::valueChanged, updateLightColor);
  connect(lightComp.colorB, &QSlider::valueChanged, updateLightColor);
}

void LightGroup::removeComponent() {
  m_entity.removeComponent<Raz::Light>();
  m_appWindow.updateLights();
}
