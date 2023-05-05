#pragma once

#include "config_base_qvariant.hpp"

#include <QFont>
#include <QString>

using namespace Qt::Literals::StringLiterals;

class WindowState final : public ConfigBaseQVariant {
  CONFIG_OPTION(QFont, TextSkinFont, u"text_skin_font"_s, QFont(u"Segoe Script"_s, 48))
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
