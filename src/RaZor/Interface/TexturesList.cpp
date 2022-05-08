#include "RaZor/Interface/TexturesList.hpp"

#include <RaZ/Data/Image.hpp>
#include <RaZ/Render/Texture.hpp>

void TexturesList::addTexture(const Raz::Texture& texture, const QString& label) {
  QPixmap pixmap(iconSize());

  const Raz::Image& img = texture.getImage();

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

  addItem(new QListWidgetItem(QIcon(pixmap), label));
}
