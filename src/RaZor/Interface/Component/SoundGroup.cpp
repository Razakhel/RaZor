#include "RaZor/Interface/Component/SoundGroup.hpp"
#include "ui_SoundComp.h"

#include <RaZ/Entity.hpp>
#include <RaZ/Audio/Sound.hpp>
#include <RaZ/Data/WavFormat.hpp>
#include <RaZ/Utils/FilePath.hpp>
#include <RaZ/Utils/Logger.hpp>
#include <RaZ/Utils/StrUtils.hpp>

SoundGroup::SoundGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::SoundComp soundComp {};
  soundComp.setupUi(this);

  auto& sound = entity.getComponent<Raz::Sound>();

  // Format & frequency

  const auto updateFormat = [&sound, formatLabel = soundComp.format] () {
    QString formatStr;

    switch (sound.getFormat()) {
      case Raz::AudioFormat::MONO_U8:
        formatStr = "Mono (8)";
        break;

      case Raz::AudioFormat::MONO_I16:
        formatStr = "Mono (16)";
        break;

      case Raz::AudioFormat::MONO_F32:
        formatStr = "Mono (32)";
        break;

      case Raz::AudioFormat::MONO_F64:
        formatStr = "Mono (64)";
        break;

      case Raz::AudioFormat::STEREO_U8:
        formatStr = "Stereo (8)";
        break;

      case Raz::AudioFormat::STEREO_I16:
        formatStr = "Stereo (16)";
        break;

      case Raz::AudioFormat::STEREO_F32:
        formatStr = "Stereo (32)";
        break;

      case Raz::AudioFormat::STEREO_F64:
        formatStr = "Stereo (64)";
        break;

      default:
        formatStr = "None";
        break;
    }

    formatLabel->setText(formatStr);
  };

  updateFormat();
  soundComp.frequency->setText(QString::number(sound.getFrequency()));

  // Repeat

  connect(soundComp.repeat, &QCheckBox::toggled, [&sound] (bool checked) { sound.setRepeat(checked); });

  // Actions

  connect(soundComp.play, &QPushButton::clicked, [&sound] () { sound.play(); });
  connect(soundComp.pause, &QPushButton::clicked, [&sound] () { sound.pause(); });
  connect(soundComp.stop, &QPushButton::clicked, [&sound] () { sound.stop(); });

  // Pitch

  const auto maxPitch = static_cast<float>(soundComp.pitch->maximum());

  soundComp.pitch->setValue(static_cast<int>(sound.recoverPitch() * maxPitch));
  connect(soundComp.pitch, &QSlider::valueChanged, [&sound, maxPitch] (int value) { sound.setPitch(static_cast<float>(value) / maxPitch); });

  // Volume (gain)

  const auto maxVolume = static_cast<float>(soundComp.volume->maximum());

  soundComp.volume->setValue(static_cast<int>(sound.recoverGain() * maxVolume));
  connect(soundComp.volume, &QSlider::valueChanged, [&sound, maxVolume] (int value) { sound.setGain(static_cast<float>(value) / maxVolume); });

  // Sound file

  connect(soundComp.soundFile, &QLineEdit::textChanged, [&sound, updateFormat, frequencyLabel = soundComp.frequency] (const QString& filePathStr) {
    const Raz::FilePath filePath = filePathStr.toStdString();
    const std::string fileExt    = Raz::StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

    if (fileExt == "wav") {
      sound = Raz::WavFormat::load(filePath);
    } else {
      Raz::Logger::error("[SoundGroup] Unrecognized audio format '" + fileExt + "'.");
      return;
    }

    updateFormat();
    frequencyLabel->setText(QString::number(sound.getFrequency()));
  });
}

void SoundGroup::removeComponent() {
  m_entity.removeComponent<Raz::Sound>();
}
