#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/TransformGroup.hpp"
#include "ui_TransformComp.h"

#include <RaZ/Math/Transform.hpp>
#include <RaZ/Render/Light.hpp>
#include <RaZ/Render/RenderSystem.hpp>

#include <QPushButton>

TransformGroup::TransformGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  assert("Error: The entity must have a Transform component to be shown." && m_entity.hasComponent<Raz::Transform>());

  Ui::TransformComp transformComp {};
  transformComp.setupUi(this);

  m_optionsButton->setDisabled(true); // Preventing component removal, which would break the app

  auto& transform = m_entity.getComponent<Raz::Transform>();
  const Raz::RenderSystem& renderSystem = appWindow.getApplication().getWorlds().back()->getSystem<Raz::RenderSystem>();

  // Position

  transformComp.positionX->setValue(static_cast<double>(transform.getPosition().x()));
  transformComp.positionY->setValue(static_cast<double>(transform.getPosition().y()));
  transformComp.positionZ->setValue(static_cast<double>(transform.getPosition().z()));

  connect(transformComp.positionX, QOverload<double>::of(&ValuePicker::valueChanged), [this, &transform, &renderSystem] (double val) {
    transform.setPosition(static_cast<float>(val), transform.getPosition().y(), transform.getPosition().z());

    if (m_entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionY, QOverload<double>::of(&ValuePicker::valueChanged), [this, &transform, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition().x(), static_cast<float>(val), transform.getPosition().z());

    if (m_entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });
  connect(transformComp.positionZ, QOverload<double>::of(&ValuePicker::valueChanged), [this, &transform, &renderSystem] (double val) {
    transform.setPosition(transform.getPosition().x(), transform.getPosition().y(), static_cast<float>(val));

    if (m_entity.hasComponent<Raz::Light>())
      renderSystem.updateLights();
  });

  // Rotation
  // TODO

  // Scale

  transformComp.scaleX->setValue(static_cast<double>(transform.getScale().x()));
  transformComp.scaleY->setValue(static_cast<double>(transform.getScale().y()));
  transformComp.scaleZ->setValue(static_cast<double>(transform.getScale().z()));

  connect(transformComp.scaleX, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(static_cast<float>(val), transform.getScale().y(), transform.getScale().z());
  });
  connect(transformComp.scaleY, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale().x(), static_cast<float>(val), transform.getScale().z());
  });
  connect(transformComp.scaleZ, QOverload<double>::of(&ValuePicker::valueChanged), [&transform] (double val) {
    transform.setScale(transform.getScale().x(), transform.getScale().y(), static_cast<float>(val));
  });
}

void TransformGroup::removeComponent() {
  m_entity.removeComponent<Raz::Transform>();
}
