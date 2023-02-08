#include "widget.hpp"

#include <QApplication>


class I {
public:
  virtual ~I() = default;
  virtual void m() = 0;
};


class D : virtual private I {
public:
  void d() {}

  I* toI() { return static_cast<I*>(this); }

private:
  void m() override {}
};


class D1 : public D {
public:
//  int m() { return 1; }
};


int main(int argc, char *argv[])
{
  D d;
  d.d();
//  d.m();

  I* i = d.toI();
  i->m();

  QApplication a(argc, argv);
  Widget w;
  w.show();
  return a.exec();
}
