/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "classic_skin.hpp"

#include <QLocale>

QString ClassicSkinConfig::default_time_format()
{
  return QLocale::system().timeFormat(QLocale::ShortFormat);
}
