#include "application.hpp"
#include "application_private.hpp"

void ApplicationPrivate::initCore()
{
  _time_src = std::make_unique<TimeSource>();
}

void Application::initCore()
{
  _impl->initCore();
}
