/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "app_state.hpp"

QDateTime AppState::GlobalState::default_update_time()
{
  return QDateTime(QDate(2013, 6, 18), QTime(11, 20, 5), QTimeZone::utc());
}
