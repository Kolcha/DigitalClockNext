#pragma once

#include <QPainter>

#include "state_guard.hpp"

template<>
inline void StateGuard<QPainter>::save(QPainter* p) { p->save(); }
template<>
inline void StateGuard<QPainter>::restore(QPainter* p) { p->restore(); }
