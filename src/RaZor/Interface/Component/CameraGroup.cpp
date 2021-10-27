#include "RaZor/Interface/Component/CameraGroup.hpp"
#include "ui_CameraComp.h"

#include <RaZ/Entity.hpp>
#include <RaZ/Render/Camera.hpp>

#include <QPushButton>

CameraGroup::CameraGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::CameraComp cameraComp;
  cameraComp.setupUi(this);

  m_optionsButton->setDisabled(true); // Preventing component removal, which would break the app

  auto& camera = m_entity.getComponent<Raz::Camera>();

  // Field of view

  cameraComp.fieldOfView->setValue(static_cast<double>(Raz::Degreesf(camera.getFieldOfView()).value));

  connect(cameraComp.fieldOfView, QOverload<double>::of(&ValuePicker::valueChanged), [&camera] (double val) {
    camera.setFieldOfView(Raz::Degreesd(val));
  });

  // Camera type

  connect(cameraComp.camType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
    camera.setCameraType((index == 0 ? Raz::CameraType::FREE_FLY : Raz::CameraType::LOOK_AT));
  });

  // Projection type

  connect(cameraComp.projType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera] (int index) {
    camera.setProjectionType((index == 0 ? Raz::ProjectionType::PERSPECTIVE : Raz::ProjectionType::ORTHOGRAPHIC));
  });
}

void CameraGroup::removeComponent() {
  m_entity.removeComponent<Raz::Camera>();
}
