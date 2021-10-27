#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/MeshGroup.hpp"
#include "ui_MeshComp.h"

#include <RaZ/Data/Mesh.hpp>

#include <QLineEdit>

MeshGroup::MeshGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::MeshComp meshComp {};
  meshComp.setupUi(this);

  auto& mesh = m_entity.getComponent<Raz::Mesh>();

  // Vertex count

  meshComp.vertexCount->setText(QString::number(mesh.recoverVertexCount()));

  // Triangle count

  meshComp.triangleCount->setText(QString::number(mesh.recoverTriangleCount()));

  // Mesh file

  connect(meshComp.meshFile, &QLineEdit::textChanged, [this, &appWindow] (const QString& filePath) {
    appWindow.importMesh(filePath.toStdString(), m_entity);
    appWindow.loadComponents();
  });
}

void MeshGroup::removeComponent() {
  m_entity.removeComponent<Raz::Mesh>();
}
