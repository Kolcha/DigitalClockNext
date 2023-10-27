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
  _updater = std::make_unique<Updater>(_app_config->global().getLastUpdateCheck());
  _updater->SetAutoupdate(_app_config->global().getCheckForUpdates());
  _updater->SetUpdatePeriod(_app_config->global().getUpdatePeriodDays());
  _updater->SetCheckForBeta(_app_config->global().getCheckForBetaVersion());

  connect(_time_src.get(), &TimeSource::timeChanged, _updater.get(), &Updater::TimeoutHandler);

  connect(_updater.get(), &Updater::UpdateChecked, this, [this](QDateTime dt) {
    _app_config->global().setLastUpdateCheck(dt);
    _app_config->global().commit();
  });
}

void Application::initCore()
{
  _impl->initCore();
}

void Application::initUpdater()
{
  _impl->initUpdater();

  using digital_clock::core::Updater;
  connect(_impl->updater().get(), &Updater::NewVersion, this, &Application::handleNewVersion);
  connect(_impl->updater().get(), &Updater::UpToDate, this, &Application::handleUpToDate);
  connect(_impl->updater().get(), &Updater::ErrorMessage, this, &Application::handleUpdateError);
}
