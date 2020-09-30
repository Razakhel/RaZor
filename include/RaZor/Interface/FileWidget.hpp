#pragma once

#ifndef RAZOR_FILEWIDGET_HPP
#define RAZOR_FILEWIDGET_HPP

#include <QLineEdit>

enum class FileType {
  MESH,
  IMAGE,
  SOUND
};

template <FileType FileT>
class FileWidget : public QLineEdit {
public:
  explicit FileWidget(QWidget* parent);

protected:
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;
};

#include "RaZor/Interface/FileWidget.inl"

#endif // RAZOR_FILEWIDGET_HPP
