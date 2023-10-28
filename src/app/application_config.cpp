#include "application.hpp"
#include "application_private.hpp"

#include <QDir>

#include "settings/backend_qsettings.hpp"

void ApplicationPrivate::initConfig()
{
  using namespace Qt::Literals::StringLiterals;
  auto backend = std::make_shared<BackendQSettings>();
#ifndef Q_OS_MACOS
  QDir app_dir(QApplication::applicationDirPath());
  if (app_dir.exists(u"portable.txt"_s) || app_dir.exists(u".portable"_s))
    backend = std::make_shared<BackendQSettings>(app_dir.absoluteFilePath(u"settings.ini"_s));
#endif
  _app_state = std::make_unique<AppState>(backend);
  _config_storage = std::make_shared<ConfigStorageType>(std::move(backend));
  _app_config = std::make_unique<AppConfig>(_config_storage);
}

void Application::initConfig()
{
  _impl->initConfig();
}
