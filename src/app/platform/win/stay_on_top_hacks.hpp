/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

class WinStayOnToHacks : public QObject
{
  Q_OBJECT

public:
  explicit WinStayOnToHacks(QObject* parent = nullptr);
  ~WinStayOnToHacks();

  // doesn't take ownership, just "observes"
  // window must be valid during whole lifetime of this object
  // or at least until the last apply() call
  void addWindow(QWidget* wnd);

public slots:
  // applies some hacks to keep window on top
  // should be called periodically, pretty often
  void apply();

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
