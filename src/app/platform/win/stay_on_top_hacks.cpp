/*
 * SPDX-FileCopyrightText: 2017-2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "stay_on_top_hacks.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <vector>

#include <QtCore/QSet>
#include <QtWidgets/QWidget>

#include "fullscreen_detect.hpp"

namespace dc = digital_clock;

struct WndHackInfo {
  QWidget* wnd            = nullptr;
  QScreen* last_screen    = nullptr;
  // list of fullscreen windows detected on the same screen
  // fullscreen transparent windows may exist on app startup
  QSet<QString> ignore_list;
};

struct WinStayOnToHacks::Impl {
  std::vector<WndHackInfo> windows;
  bool detect_fullscreen = true;
};

WinStayOnToHacks::WinStayOnToHacks(QObject* parent)
  : QObject(parent)
  , _impl(std::make_unique<Impl>())
{
}

WinStayOnToHacks::~WinStayOnToHacks() = default;

void WinStayOnToHacks::addWindow(QWidget* wnd)
{
  _impl->windows.push_back(WndHackInfo{ .wnd = wnd });
}

void WinStayOnToHacks::apply()
{
  for (auto& hi : _impl->windows) {
    // calling winId() for invisible window causes move event from (0,0) to (289,160)
    // during startup (doesn't matter what saved coordinates were, each time the same),
    // as result real saved position will be overwritten even before it will be read
    // so do nothing if window is not visible
    if (!hi.wnd->isVisible()) continue;

    // remember all fullscreen windows on app startup or window's screen change and ignore them later
    if (hi.last_screen != hi.wnd->screen()) {
      hi.last_screen = hi.wnd->screen();
      hi.ignore_list = dc::GetFullscreenWindowsOnSameMonitor(hi.wnd->winId());
    }

    if (_impl->detect_fullscreen && dc::IsFullscreenWndOnSameMonitor(hi.wnd->winId(), hi.ignore_list)) {
      // don't stay above fullscreen windows
      if ((GetWindowLongPtr((HWND)hi.wnd->winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0) {
        SetWindowPos((HWND)hi.wnd->winId(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
      }
    } else {
      // always on top problem workaround
      // sometimes even window have Qt::WindowStaysOnTopHint window flag, it doesn't have WS_EX_TOPMOST flag,
      // so set it manually using WinAPI...
      if ((GetWindowLongPtr((HWND)hi.wnd->winId(), GWL_EXSTYLE) & WS_EX_TOPMOST) == 0) {
        SetWindowPos((HWND)hi.wnd->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
      }
      // keep above the taskbar workaround
      // https://forum.qt.io/topic/28739/flags-windows-7-window-always-on-top-including-the-win7-taskbar-custom-error/4
      if (!hi.wnd->isActiveWindow()) hi.wnd->raise();
    }
  }
}
