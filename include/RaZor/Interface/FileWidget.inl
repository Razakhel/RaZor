#include <QDropEvent>
#include <QMimeData>

#include <iostream>

namespace {

template <FileType FileT>
bool isValid(const QString& filePath) {
  const QStringRef fileExt = filePath.splitRef('.').back();

  if constexpr (FileT == FileType::MESH) {
    if (fileExt == "obj" || fileExt == "fbx" || fileExt == "off")
      return true;
  } else if constexpr (FileT == FileType::IMAGE) {
    if (fileExt == "png" || fileExt == "tga")
      return true;
  }

  return false;
}

} // namespace

template <FileType FileT>
void FileWidget<FileT>::dragEnterEvent(QDragEnterEvent* event) {
  if (!isValid<FileT>(event->mimeData()->text()))
    return;

  event->setDropAction(Qt::DropAction::LinkAction);
  event->accept();
}

template <FileType FileT>
void FileWidget<FileT>::dropEvent(QDropEvent* event) {
  const QString filePath = event->mimeData()->text().remove(0, 8); // The text is prepended with "file:///", which must be removed

  if (!isValid<FileT>(filePath)) {
    std::cerr << "[RaZor] " << tr("Error: ").toStdString() << tr("Unrecognized file type").toStdString() << std::endl;
    return;
  }

  setText(filePath);
}
