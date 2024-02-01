/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>

#include <QDateTime>

#include "resource.hpp"
#include "observable.hpp"
#include "skin_visitor.hpp"

class SkinObserver {
public:
  virtual ~SkinObserver() = default;

  virtual void onConfigurationChanged() = 0;
};

class Skin : public Observable<SkinObserver> {
public:
  virtual ~Skin() = default;

  virtual std::shared_ptr<Resource> process(const QDateTime& dt) = 0;

  virtual void setSeparatorAnimationEnabled(bool enabled) = 0;
  inline void EnableSeparatorAnimation() { setSeparatorAnimationEnabled(true); }
  inline void DisableSeparatorAnimation() { setSeparatorAnimationEnabled(false); }

  virtual void animateSeparator() = 0;

  virtual void visit(SkinVisitor& visitor) = 0;

protected:
  // implementations should call this to notify about its configuration change
  // it also should be called when geometry is changed too
  void configurationChanged() const { notify(&SkinObserver::onConfigurationChanged); }
};
