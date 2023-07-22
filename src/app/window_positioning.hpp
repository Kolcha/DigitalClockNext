#pragma once

#include <QRect>

QPoint pick_ref_point(const QRect& r, Qt::Alignment a) noexcept;
QRect aligned_rect(const QRect& r, const QPoint& p, Qt::Alignment a) noexcept;
