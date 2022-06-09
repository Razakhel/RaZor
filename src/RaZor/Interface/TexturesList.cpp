#include "RaZor/Interface/TexturesList.hpp"

#include <RaZ/Data/Image.hpp>
#include <RaZ/Render/Texture.hpp>
#include <RaZ/Utils/Logger.hpp>

void TexturesList::addTexture(const Raz::Texture& texture, const QString& label) {
  const auto* texture2D = dynamic_cast<const Raz::Texture2D*>(&texture);

  if (texture2D == nullptr) {
    Raz::Logger::error("[TexturesList] Non-2D textures cannot be displayed yet.");
    return;
  }

  QPixmap pixmap(iconSize());
  const Raz::Image img = texture2D->recoverImage();

  if (!img.isEmpty()) {
    QImage::Format format {};

    switch (img.getColorspace()) {
      case Raz::ImageColorspace::GRAY:
        format = QImage::Format::Format_Grayscale8;
        break;

      case Raz::ImageColorspace::GRAY_ALPHA:
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
        format = QImage::Format::Format_Grayscale16; // Most likely invalid, but no specific format exists for this
#endif
        break;

      case Raz::ImageColorspace::RGB:
      default:
        format = QImage::Format::Format_RGB888;
        break;

      case Raz::ImageColorspace::RGBA:
        format = QImage::Format::Format_RGBA8888;
        break;
    }

    pixmap.convertFromImage(QImage(static_cast<const uint8_t*>(img.getDataPtr()),
                                   static_cast<int>(img.getWidth()), static_cast<int>(img.getHeight()),
                                   static_cast<int>(img.getChannelCount() * img.getWidth()), format));
  } else {
    pixmap.fill(Qt::white);
  }

  addItem(new QListWidgetItem(QIcon(pixmap), label));
}
