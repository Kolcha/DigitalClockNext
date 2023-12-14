#pragma once

#include <memory>
#include <vector>

template<class Observer>
class Observable {
public:
  void addObserver(std::weak_ptr<Observer> observer)
  {
    _observers.push_back(std::move(observer));
    std::erase_if(_observers, [](auto& wo) noexcept { return !wo.lock(); });
  }

protected:
  template<typename Method, typename... Args>
  void notify(Method method, Args&&... args) const
  {
    for (const auto& wo : _observers) {
      if (auto o = wo.lock()) {
        (*o.*method)(std::forward<Args>(args)...);
      }
    }
  }

private:
  std::vector<std::weak_ptr<Observer>> _observers;
};
