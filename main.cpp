#include "widget.hpp"

#include "app/application.hpp"

int main(int argc, char *argv[])
{
  Application a(argc, argv);
  a.init();
  Widget w;
/*
  w.setWindowFlag(Qt::FramelessWindowHint);
  w.setAttribute(Qt::WA_TranslucentBackground);
  w.showMaximized();
*/
  w.show();
  return a.exec();
}
