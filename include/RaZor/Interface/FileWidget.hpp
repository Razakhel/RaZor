#pragma once

#ifndef RAZOR_FILEWIDGET_HPP
#define RAZOR_FILEWIDGET_HPP

#include <QLineEdit>

enum class FileType {
  MESH,
  IMAGE
};

template <FileType FileT>
class FileWidget : public QLineEdit {
public:
  explicit FileWidget(QWidget* parent) : QLineEdit(parent) { setAcceptDrops(true); }

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;
};

#include "RaZor/Interface/FileWidget.inl"

#endif // RAZOR_FILEWIDGET_HPP
