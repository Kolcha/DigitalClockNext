#include "application.hpp"
#include "application_private.hpp"

#include "settings/backend_qsettings.hpp"

void ApplicationPrivate::initConfig()
{
  using namespace Qt::Literals::StringLiterals;
  // TODO: use file for now, switch to default later
  // TODO: remove using namespace from config headers
  auto backend = std::make_unique<BackendQSettings>(u"settings.ini"_s);
  _config_storage = std::make_shared<ConfigStorageType>(std::move(backend));
  _app_config = std::make_unique<AppConfig>(_config_storage);
}

void Application::initConfig()
{
  _impl->initConfig();
}
