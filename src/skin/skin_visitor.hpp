/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

// only forward declarations, this header should not include everything
class ClassicSkin;
class ErrorSkin;

class SkinVisitor {
public:
  virtual ~SkinVisitor() = default;

  virtual void visit(ClassicSkin*) = 0;
  virtual void visit(ErrorSkin*) = 0;
};
