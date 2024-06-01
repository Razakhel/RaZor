#include <QDropEvent>
#include <QFileDialog>
#include <QMimeData>

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
#include <QStringView>
#else
#include <QStringRef>
#endif

#include <iostream>

namespace {

template <FileType FileT>
std::vector<std::string_view> recoverFileFormats() {
  if constexpr (FileT == FileType::IMAGE)
    return { "bmp", "gif", "hdr", "jpeg", "jpg", "pgm", "pic", "png", "ppm", "psd", "tga" };
  else if constexpr (FileT == FileType::LUA_SCRIPT)
    return { "lua" };
  else if constexpr (FileT == FileType::MESH)
    return { "fbx", "glb", "gltf", "obj", "off" };
  else if constexpr (FileT == FileType::SHADER)
    return { "comp", "frag", "geom", "glsl", "tesc", "tese", "vert" };
  else if constexpr (FileT == FileType::SOUND)
    return { "wav" };
}

template <FileType FileT>
bool isValid(const QString& filePath) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
  const QStringView fileExt = QStringView(filePath).split('.').back();
#else
  const QStringRef fileExt = filePath.splitRef('.').back();
#endif

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
  setPlaceholderText("No file");
}

template <FileType FileT>
void FileWidget<FileT>::enterEvent(QEvent* event) {
  setCursor(Qt::CursorShape::PointingHandCursor);
  event->accept();
}

template <FileType FileT>
void FileWidget<FileT>::leaveEvent(QEvent* event) {
  setCursor(Qt::CursorShape::ArrowCursor);
  event->accept();
}

template <FileType FileT>
void FileWidget<FileT>::mousePressEvent(QMouseEvent* event) {
  if (event->button() != Qt::MouseButton::LeftButton)
    return;

  const std::vector<std::string_view> formats = recoverFileFormats<FileT>();

  QString formatsStr;

  if constexpr (FileT == FileType::IMAGE)
    formatsStr += tr("Image");
  else if constexpr (FileT == FileType::LUA_SCRIPT)
    formatsStr += tr("Lua script");
  else if constexpr (FileT == FileType::MESH)
    formatsStr += tr("Mesh");
  else if constexpr (FileT == FileType::SHADER)
    formatsStr += "Shader";
  else if constexpr (FileT == FileType::SOUND)
    formatsStr += tr("Sound");

  formatsStr += " (";
  for (const std::string_view& format : formats)
    formatsStr += "*." + QString(format.data()) + ' ';
  formatsStr.back() = ')';

  const QString filePath = QFileDialog::getOpenFileName(this, tr("Import a file"), QString(), formatsStr);

  if (filePath.isEmpty())
    return;

  setText(filePath);
  event->accept();
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
  event->accept();
}
