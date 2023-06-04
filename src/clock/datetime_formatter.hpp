#pragma once

#include <memory>

#include <QHash>
#include <QDateTime>
#include <QString>

class DateTimeFormatter {
public:
  class Extension {
  public:
    virtual ~Extension() = default;

    virtual QString name() const = 0;

    void enable() noexcept { _enabled = true; }
    void disable() noexcept { _enabled = false; }

    void setEnabled(bool enabled) noexcept { _enabled = enabled; }
    bool isEnabled() const noexcept { return _enabled; }

    QString operator ()(const QString& fmt,
                        const QString& seps,
                        const QDateTime& dt)
    {
      if (!_enabled)
        return fmt;
      return process(fmt, seps, dt);
    }

  protected:
    virtual QString process(const QString& fmt,
                            const QString& seps,
                            const QDateTime& dt) = 0;

  private:
    bool _enabled = true;
  };

  explicit DateTimeFormatter(QString format)
    : _format(std::move(format))
    , _separators(".:/|*-")
  {}

  QString process(const QDateTime& dt) const
  {
    // TODO: extended formats support,
    // TODO: e.g. week/day number and etc.
    auto fmt = _format;
    for (const auto& ext : _extensions)
      fmt = (*ext)(fmt, _separators, dt);
    return dt.toString(fmt);
  }

  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
  }

  QString format() const { return _format; }

  bool isSeparator(QChar ch) const
  {
    return _separators.contains(ch);
  }

  void setSeparators(QString seps)
  {
    if (seps.isEmpty() || seps == _separators)
      return;
    _separators = std::move(seps);
  }

  void addExtension(std::shared_ptr<Extension> ext)
  {
    auto key = ext->name();
    _extensions[key] = std::move(ext);
  }

  void setExtensionEnabled(const QString& name, bool enabled)
  {
    auto iter = _extensions.find(name);
    if (iter != _extensions.end())
      iter.value()->setEnabled(enabled);
  }

private:
  QString _format;
  QString _separators;
  QHash<QString, std::shared_ptr<Extension>> _extensions;
};
