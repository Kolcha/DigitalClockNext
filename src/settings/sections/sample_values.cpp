#include "sample_values.hpp"

QGradient sample_conical_gradient()
{
  QConicalGradient g(0.5, 0.5, 45.0);
  g.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QGradient sample_linear_gradient()
{
  QLinearGradient g(0., 0., 0., 1.);
  g.setColorAt(0.0, Qt::transparent);
  g.setColorAt(0.3, Qt::transparent);
  g.setColorAt(1.0, Qt::magenta);
  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}
