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
    const Raz::Material& material = meshRenderer.getMaterials()[materialIndex];

    if (material.getProgram().getTextureCount() == 0)
      continue;

    meshRendererComp.materials->addTexture(material.getProgram().getTexture(0), "Material #" + QString::number(materialIndex));
  }

  connect(meshRendererComp.addMaterial, &QPushButton::clicked, [&meshRenderer, &appWindow] () {
    meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE));
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
      meshRenderer.setMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE));

    appWindow.loadComponents();
  });
}

void MeshRendererGroup::removeComponent() {
  m_entity.removeComponent<Raz::MeshRenderer>();
}
