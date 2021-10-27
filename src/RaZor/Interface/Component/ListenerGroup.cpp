#include "RaZor/Interface/Component/ListenerGroup.hpp"
#include "ui_ListenerComp.h"

#include <RaZ/Entity.hpp>
#include <RaZ/Audio/Listener.hpp>

#include <QPushButton>

ListenerGroup::ListenerGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::ListenerComp listenerComp {};
  listenerComp.setupUi(this);

  m_optionsButton->setDisabled(true); // Preventing component removal, which would break the app

  auto& listener = m_entity.getComponent<Raz::Listener>();

  // Position

  const Raz::Vec3f position = listener.recoverPosition();

  listenerComp.positionX->setValue(static_cast<double>(position.x()));
  listenerComp.positionY->setValue(static_cast<double>(position.y()));
  listenerComp.positionZ->setValue(static_cast<double>(position.z()));

  // Velocity

  const Raz::Vec3f velocity = listener.recoverVelocity();

  listenerComp.velocityX->setValue(static_cast<double>(velocity.x()));
  listenerComp.velocityY->setValue(static_cast<double>(velocity.y()));
  listenerComp.velocityZ->setValue(static_cast<double>(velocity.z()));

  // Orientation

  const Raz::Vec3f forward = listener.recoverForwardOrientation();

  listenerComp.forwardX->setValue(static_cast<double>(forward.x()));
  listenerComp.forwardY->setValue(static_cast<double>(forward.y()));
  listenerComp.forwardZ->setValue(static_cast<double>(forward.z()));

  const Raz::Vec3f up = listener.recoverUpOrientation();

  listenerComp.upX->setValue(static_cast<double>(up.x()));
  listenerComp.upY->setValue(static_cast<double>(up.y()));
  listenerComp.upZ->setValue(static_cast<double>(up.z()));
}

void ListenerGroup::removeComponent() {
  m_entity.removeComponent<Raz::Listener>();
}
