#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/MeshRendererGroup.hpp"
#include "ui_MeshRendererComp.h"

#include <RaZ/Data/Mesh.hpp>
#include <RaZ/Render/MeshRenderer.hpp>

MeshRendererGroup::MeshRendererGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::MeshRendererComp meshRendererComp {};
  meshRendererComp.setupUi(this);

  auto& meshRenderer = entity.getComponent<Raz::MeshRenderer>();

  // Render mode

  // If no mesh available or is empty, prevent changing render mode (indices can't be loaded)
  meshRendererComp.renderMode->setEnabled(entity.hasComponent<Raz::Mesh>() && !entity.getComponent<Raz::Mesh>().getSubmeshes().empty());

  meshRendererComp.renderMode->setCurrentIndex((meshRenderer.getSubmeshRenderers().front().getRenderMode() == Raz::RenderMode::TRIANGLE ? 0 : 1));

  connect(meshRendererComp.renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [&meshRenderer, &entity] (int index) {
    meshRenderer.setRenderMode((index == 0 ? Raz::RenderMode::TRIANGLE : Raz::RenderMode::POINT), entity.getComponent<Raz::Mesh>());
  });

  // Materials

  for (std::size_t materialIndex = 0; materialIndex < meshRenderer.getMaterials().size(); ++materialIndex) {
    QPixmap pixmap(meshRendererComp.materials->iconSize());

    const Raz::Image& img = meshRenderer.getMaterials()[materialIndex]->getBaseColorMap()->getImage();

    if (!img.isEmpty()) {
      uint8_t channelCount {};
      QImage::Format format {};

      switch (img.getColorspace()) {
        case Raz::ImageColorspace::DEPTH:
        case Raz::ImageColorspace::GRAY:
          channelCount = 1;
          format = QImage::Format::Format_Grayscale8;
          break;

        case Raz::ImageColorspace::GRAY_ALPHA:
          channelCount = 2;
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
          format = QImage::Format::Format_Grayscale16; // Most likely invalid, but no specific format exists for this
#endif
          break;

        case Raz::ImageColorspace::RGB:
        default:
          channelCount = 3;
          format = QImage::Format::Format_RGB888;
          break;

        case Raz::ImageColorspace::RGBA:
          channelCount = 4;
          format = QImage::Format::Format_RGBA8888;
          break;
      }

      pixmap.convertFromImage(QImage(static_cast<const uint8_t*>(img.getDataPtr()),
                                     static_cast<int>(img.getWidth()), static_cast<int>(img.getHeight()),
                                     static_cast<int>(channelCount * img.getWidth()), format));
    } else {
      pixmap.fill(Qt::white);
    }

    meshRendererComp.materials->addItem(new QListWidgetItem(QIcon(pixmap), "Material #" + QString::number(materialIndex)));
  }

  connect(meshRendererComp.addMaterial, &QPushButton::clicked, [&meshRenderer, &appWindow] () {
    meshRenderer.addMaterial(Raz::MaterialCookTorrance::create());
    appWindow.loadComponents();
  });

  meshRendererComp.removeMaterial->setEnabled(meshRendererComp.materials->count() > 0);

  connect(meshRendererComp.removeMaterial, &QPushButton::clicked, [meshRendererComp, &meshRenderer, &appWindow] () {
    // Removing all selected materials
    for (const QListWidgetItem* item : meshRendererComp.materials->selectedItems()) {
      const int itemRowIndex = meshRendererComp.materials->row(item);

      meshRenderer.removeMaterial(static_cast<std::size_t>(itemRowIndex));
      delete meshRendererComp.materials->takeItem(itemRowIndex);
    }

    // If we've just deleted the last material, set a default one
    if (meshRenderer.getMaterials().empty())
      meshRenderer.setMaterial(Raz::MaterialCookTorrance::create());

    appWindow.loadComponents();
  });
}

void MeshRendererGroup::removeComponent() {
  m_entity.removeComponent<Raz::MeshRenderer>();
}
