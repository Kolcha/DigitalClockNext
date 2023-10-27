/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2017-2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "build_date.hpp"

namespace digital_clock {
namespace core {

QDate build_date()
{
  QString build_date = QString::fromLatin1(__DATE__);
  // date is space-padded instead of zero-padded
  if (build_date[4].isSpace()) build_date[4] = QLatin1Char('0');
  return QDate::fromString(build_date, QLatin1String("MMM dd yyyy"));
}

} // namespace core
} // namespace digital_clock