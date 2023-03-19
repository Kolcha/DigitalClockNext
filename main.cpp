#include "widget.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Widget w;
/*
  w.setWindowFlag(Qt::FramelessWindowHint);
  w.setAttribute(Qt::WA_TranslucentBackground);
  w.showMaximized();
*/
  w.show();
  return a.exec();
}
