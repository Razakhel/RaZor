#pragma once

#ifndef RAZOR_TEXTURESLIST_HPP
#define RAZOR_TEXTURESLIST_HPP

#include <QListWidget>

namespace Raz { class Texture; }

class TexturesList final : public QListWidget {
public:
  explicit TexturesList(QWidget* parent) : QListWidget(parent) {}

  void addTexture(const Raz::Texture& texture, const QString& label);
};

#endif // RAZOR_TEXTURESLIST_HPP
