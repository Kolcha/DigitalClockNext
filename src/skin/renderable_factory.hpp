#pragma once

#include <memory>

#include <QChar>
#include <QString>

#include "render/skin_resource.hpp"

class RenderableFactory {
public:
  virtual ~RenderableFactory() = default;

  virtual std::shared_ptr<SkinResource> item(QChar ch) const = 0;

  void setSeparators(QString seps) { _seps = std::move(seps); }
  QString separators() const { return _seps; }

  virtual bool isSeparator(QChar ch) const
  {
    return _seps.isEmpty() ? !ch.isLetterOrNumber() : _seps.contains(ch);
  }

private:
  QString _seps;
};
