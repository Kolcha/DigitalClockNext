#include "application.hpp"
#include "application_private.hpp"

#include "skin_manager.hpp"

void ApplicationPrivate::initCore()
{
  _time_src = std::make_unique<TimeSource>();
  _skin_manager = std::make_unique<SkinManagerImpl>(this);
}

void Application::initCore()
{
  _impl->initCore();
}
