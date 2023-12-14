/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include <QDateTime>

#include "glyph.hpp"
#include "observable.hpp"

class SkinObserver {
public:
  virtual ~SkinObserver() = default;

  virtual void onConfigurationChanged() = 0;
};

class Skin : public Observable<SkinObserver> {
public:
  virtual ~Skin() = default;

  virtual std::shared_ptr<Glyph> process(const QDateTime& dt) = 0;

  virtual void setSeparatorAnimationEnabled(bool enabled) = 0;
  inline void EnableSeparatorAnimation() { setSeparatorAnimationEnabled(true); }
  inline void DisableSeparatorAnimation() { setSeparatorAnimationEnabled(false); }

  virtual void animateSeparator() = 0;

protected:
  // implementations should call this to notify about its configuration change
  // it also should be called when geometry is changed too
  void configurationChanged() const { notify(&SkinObserver::onConfigurationChanged); }
};
