/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QRect>

QPoint pick_ref_point(const QRect& r, Qt::Alignment a) noexcept;
QRect aligned_rect(const QRect& r, const QPoint& p, Qt::Alignment a) noexcept;
