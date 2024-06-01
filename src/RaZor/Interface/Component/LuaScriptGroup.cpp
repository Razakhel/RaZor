#include "RaZor/Interface/AppWindow.hpp"
#include "RaZor/Interface/Component/LuaScriptGroup.hpp"
#include "ui_LuaScriptComp.h"

#include <RaZ/Script/LuaScript.hpp>
#include <RaZ/Utils/FilePath.hpp>
#include <RaZ/Utils/Logger.hpp>

#include <QTextStream>

LuaScriptGroup::LuaScriptGroup(Raz::Entity& entity, AppWindow& appWindow) : ComponentGroup(entity, appWindow) {
  Ui::LuaScriptComp luaScriptComp {};
  luaScriptComp.setupUi(this);

  auto& luaScript = entity.getComponent<Raz::LuaScript>();

  // Script file

  connect(luaScriptComp.luaScriptFile, &QLineEdit::textChanged, [&entity, &luaScript] (const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      Raz::Logger::error("[LuaScript] " + tr("Could not open the file").toStdString() + '\'' + filePath.toStdString() + '\'');
      return;
    }

    const std::string fileContent = QTextStream(&file).readAll().toStdString();

    try {
      // Running a dummy script, checking that it's syntactically correct
      Raz::LuaScript testScript(fileContent);
      testScript.registerEntity(entity, "this");
      testScript.update({});

      luaScript.loadCode(fileContent);
    } catch (const std::exception& ex) {
      Raz::Logger::error("[LuaScript] " + tr("Failed to load the script:").toStdString() + '\n' + ex.what());
    }
  });
}

void LuaScriptGroup::removeComponent() {
  m_entity.removeComponent<Raz::LuaScript>();
}
