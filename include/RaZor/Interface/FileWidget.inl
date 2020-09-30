#include <QDropEvent>
#include <QMimeData>

#include <iostream>

namespace {

template <FileType FileT>
std::vector<std::string_view> recoverFileFormats() {
  if constexpr (FileT == FileType::MESH)
    return { "obj", "fbx", "off" };
  else if constexpr (FileT == FileType::IMAGE)
    return { "png", "tga" };
  else if constexpr (FileT == FileType::SOUND)
    return { "wav" };
}

template <FileType FileT>
bool isValid(const QString& filePath) {
  const QStringRef fileExt = filePath.splitRef('.').back();
  const std::vector<std::string_view> formats = recoverFileFormats<FileT>();

  if (std::find(formats.cbegin(), formats.cend(), fileExt.toString().toStdString()) != formats.cend())
    return true;

  return false;
}

} // namespace

template <FileType FileT>
FileWidget<FileT>::FileWidget(QWidget* parent) : QLineEdit(parent) {
  setAcceptDrops(true);
  setReadOnly(true);
}

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
