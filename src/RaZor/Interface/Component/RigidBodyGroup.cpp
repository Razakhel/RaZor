#include "RaZor/Interface/Component/RigidBodyGroup.hpp"
#include "ui_RigidBodyComp.h"

#include <RaZ/Entity.hpp>
#include <RaZ/Physics/RigidBody.hpp>

RigidBodyGroup::RigidBodyGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::RigidBodyComp rigidBodyComp {};
  rigidBodyComp.setupUi(this);

  auto& rigidBody = entity.getComponent<Raz::RigidBody>();

  // Mass

  rigidBodyComp.mass->setValue(static_cast<double>(rigidBody.getMass()));

  connect(rigidBodyComp.mass, QOverload<double>::of(&ValuePicker::valueChanged), [&rigidBody] (double val) {
    rigidBody.setMass(static_cast<float>(val));
  });

  // Bounciness

  rigidBodyComp.bounciness->setValue(static_cast<double>(rigidBody.getBounciness()));

  connect(rigidBodyComp.bounciness, QOverload<double>::of(&ValuePicker::valueChanged), [&rigidBody] (double val) {
    rigidBody.setBounciness(static_cast<float>(val));
  });

  // Velocity

  rigidBodyComp.velocityX->setValue(static_cast<double>(rigidBody.getVelocity().x()));
  rigidBodyComp.velocityY->setValue(static_cast<double>(rigidBody.getVelocity().y()));
  rigidBodyComp.velocityZ->setValue(static_cast<double>(rigidBody.getVelocity().z()));

  const auto updateVelocity = [rigidBodyComp, &rigidBody] (double) {
    const auto velocityX = static_cast<float>(rigidBodyComp.velocityX->value());
    const auto velocityY = static_cast<float>(rigidBodyComp.velocityY->value());
    const auto velocityZ = static_cast<float>(rigidBodyComp.velocityZ->value());

    rigidBody.setVelocity(Raz::Vec3f(velocityX, velocityY, velocityZ));
  };

  connect(rigidBodyComp.velocityX, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityY, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);
  connect(rigidBodyComp.velocityZ, QOverload<double>::of(&ValuePicker::valueChanged), updateVelocity);
}

void RigidBodyGroup::removeComponent() {
  m_entity.removeComponent<Raz::RigidBody>();
}
