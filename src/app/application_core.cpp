#include "application.hpp"
#include "application_private.hpp"

#include "skin_manager.hpp"

void ApplicationPrivate::initCore()
{
  _time_src = std::make_unique<TimeSource>();
  _skin_manager = std::make_unique<SkinManagerImpl>(this);
}

void ApplicationPrivate::initUpdater()
{
  _update_checker = std::make_unique<UpdateChecker>(_app_state->global().getLastUpdateCheck());
  _update_checker->setAutoupdate(_app_config->global().getCheckForUpdates());
  _update_checker->setUpdatePeriod(_app_config->global().getUpdatePeriodDays());
  _update_checker->setCheckForBeta(_app_config->global().getCheckForBetaVersion());

  connect(_time_src.get(), &TimeSource::timeChanged, _update_checker.get(), &UpdateChecker::timerHandler);

  connect(_update_checker.get(), &UpdateChecker::updateChecked, this, [this](QDateTime dt) {
    _app_state->global().setLastUpdateCheck(dt);
  });
}

void Application::initCore()
{
  _impl->initCore();
}

void Application::initUpdater()
{
  _impl->initUpdater();

  connect(_impl->update_checker().get(), &UpdateChecker::newVersion, this, &Application::handleNewVersion);
  connect(_impl->update_checker().get(), &UpdateChecker::upToDate, this, &Application::handleUpToDate);
  connect(_impl->update_checker().get(), &UpdateChecker::errorMessage, this, &Application::handleUpdateError);
}
