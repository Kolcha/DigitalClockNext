#pragma once

#include "config_base_qvariant.hpp"

#include <QColor>
#include <QGradient>
#include <QFont>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QTimeZone>

#include "sample_values.hpp"

class WindowState final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QPoint, Pos, QPoint(75, 50))
  CONFIG_OPTION_Q(Qt::Alignment, Alignment, Qt::AlignTop | Qt::AlignLeft)
  CONFIG_OPTION_Q(QFont, TextSkinFont, QFont("Segoe Script", 48))
  CONFIG_OPTION_Q(QString, LastUsedSkin, QString("Floral digits"))
  CONFIG_OPTION_Q(QColor, TextureColor, QColor(112, 96, 240))
  CONFIG_OPTION_Q(QGradient, TextureGradient, sample_conical_gradient())
  CONFIG_OPTION_Q(QPixmap, TexturePattern, QPixmap())
  CONFIG_OPTION_Q(QColor, BackgroundColor, QColor(0, 0, 0, 160))
  CONFIG_OPTION_Q(QGradient, BackgroundGradient, sample_linear_gradient())
  CONFIG_OPTION_Q(QPixmap, BackgroundPattern, QPixmap())
  CONFIG_OPTION_Q(QTimeZone, TimeZone, QTimeZone::systemTimeZone())
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
