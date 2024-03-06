/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "modern_skin.hpp"

#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "classic_skin.hpp"
#include "effects.hpp"
#include "font_resource.hpp"
#include "image_resource.hpp"
#include "legacy_skin_loader.hpp"
#include "layout.hpp"
#include "linear_layout.hpp"

namespace {

std::unique_ptr<Resource> createResource(const QString& filename)
{
  QFileInfo fi(filename);

  if (!fi.exists())
    return nullptr;

  auto ext = fi.suffix().toLower();
  if (ext == "png")
    return std::make_unique<RasterImageResource>(filename);
  if (ext == "svg")
    return std::make_unique<SvgImageResource>(filename);

  return nullptr;
}


// helper RAII object to chage current path
class PWDChanger final {
public:
  explicit PWDChanger(const QString& path)
    : _last_pwd(QDir::currentPath())
  {
    QDir::setCurrent(path);
  }

  ~PWDChanger()
  {
    QDir::setCurrent(_last_pwd);
  }

private:
  QString _last_pwd;
};


// skin's lifetime must match item's lifetime
class StaticTextItem final : public LayoutItem {
public:
  StaticTextItem(std::unique_ptr<StaticText> skin, QStringView text)
    : LayoutItem(skin->process(text))
    , _skin(std::move(skin))
  {}

private:
  std::unique_ptr<StaticText> _skin;
};


class SkinItem : public LayoutItem {
public:
  SkinItem(std::shared_ptr<Skin> skin, const QDateTime& dt)
    : SkinItem(std::make_shared<SkinResource>(std::move(skin), dt))
  {}

  void process(const QDateTime& dt)
  {
    _res->process(dt);
    updateGeometry();
  }

  std::shared_ptr<Skin> skin() const noexcept { return _res->skin(); }

private:
  class SkinResource : public Resource {
  public:
    SkinResource(std::shared_ptr<Skin> skin, const QDateTime& dt)
      : _skin(std::move(skin))
      , _res(_skin->process(dt))
    {}

    QRectF rect() const override { return _res->rect(); }
    qreal advanceX() const override { return _res->advanceX(); }
    qreal advanceY() const override { return _res->advanceY(); }
    void draw(QPainter* p) override { _res->draw(p); }

    size_t cacheKey() const override { return _res->cacheKey(); }

    void process(const QDateTime& dt) { _res = _skin->process(dt); }

    std::shared_ptr<Skin> skin() const noexcept { return _skin; }

  private:
    std::shared_ptr<Skin> _skin;
    std::shared_ptr<Resource> _res;
  };

private:
  SkinItem(std::shared_ptr<SkinResource> res)
    : LayoutItem(res)
    , _res(res)
  {}

private:
  std::shared_ptr<SkinResource> _res;
};


class ModernLayout : public Layout {
public:
protected:
  void doAddItem(std::shared_ptr<LayoutItem> item) override
  {
    Q_UNUSED(item);
  }

  std::pair<qreal, qreal> doBuildLayout() override
  {
    return {0, 0}; // TODO: retunrn (w, h)
  }
};

// serialization
QFont parseFont(const QJsonObject& js)
{
  QFont font;
  if (const auto v = js["family"]; v.isString())
    font.setFamily(v.toString());
  if (const auto v = js["size"]; v.isDouble())
    font.setPointSizeF(v.toDouble());
  if (const auto v = js["bold"]; v.isBool())
    font.setBold(v.toBool());
  if (const auto v = js["italic"]; v.isBool())
    font.setItalic(v.toBool());
  return font;
}

QRectF parseRect(const QJsonObject& js)
{
  QRectF r;
  if (const auto v = js["x"]; v.isDouble())
    r.setX(v.toDouble());
  if (const auto v = js["y"]; v.isDouble())
    r.setY(v.toDouble());
  if (const auto v = js["w"]; v.isDouble())
    r.setWidth(v.toDouble());
  if (const auto v = js["h"]; v.isDouble())
    r.setHeight(v.toDouble());
  return r;
}

QPointF parsePoint(const QJsonObject& js)
{
  QPointF p;
  if (const auto v = js["x"]; v.isDouble())
    p.setX(v.toDouble());
  if (const auto v = js["y"]; v.isDouble())
    p.setY(v.toDouble());
  return p;
}

QTransform parseTransform(const QJsonObject& js)
{
  QTransform t;

  if (const auto v = js["rotate"]; v.isDouble())
    t.rotate(v.toDouble());

  qreal sx = 1;
  qreal sy = 1;

  if (const auto v = js["scale"]; v.isDouble())
    sx = sy = v.toDouble();

  if (const auto v = js["scaleX"]; v.isDouble())
    sx = v.toDouble();
  if (const auto v = js["scaleY"]; v.isDouble())
    sy = v.toDouble();

  t.scale(sx, sy);

  return t;
}

Qt::Alignment parseHAlign(const QString& str)
{
  if (str == "left")
    return Qt::AlignLeft;
  if (str == "center")
    return Qt::AlignHCenter;
  if (str == "right")
    return Qt::AlignRight;
  return Qt::AlignJustify;
}

Qt::Alignment parseVAlign(const QString& str)
{
  if (str == "top")
    return Qt::AlignTop;
  if (str == "center")
    return Qt::AlignVCenter;
  if (str == "bottom")
    return Qt::AlignBottom;
  return Qt::AlignBaseline;
}

Qt::Alignment parseAlignment(const QJsonObject& js)
{
  Qt::Alignment halign = Qt::AlignJustify;
  Qt::Alignment valign = Qt::AlignBaseline;
  if (const auto v = js["halign"]; v.isString())
    halign = parseHAlign(v.toString());
  if (const auto v = js["valign"]; v.isString())
    valign = parseVAlign(v.toString());
  return halign | valign;
}

std::unique_ptr<ClassicSkin> parseFontSkin(const QJsonObject& js)
{
  auto provider = std::make_unique<FontResourceFactory>(parseFont(js));
  auto skin = std::make_unique<ClassicSkin>(std::move(provider));
  skin->setSupportsGlyphBaseHeight(false);
  skin->setSupportsCustomSeparator(true);
  return skin;
}

void parseCommonLayoutItemParams(const QJsonObject& js, LayoutItem& item)
{
  if (const auto v = js["pos"]; v.isObject())
    item.setPos(parsePoint(v.toObject()));
  // transform is described by set of item's keys
  item.setTransform(parseTransform(js));
  if (const auto v = js["resize"]; v.isBool())
    item.setResizeEnabled(v.toBool());
}

QGradient::Type parseGradientType(const QString& str)
{
  if (str == "linear")
    return QGradient::LinearGradient;
  if (str == "radial")
    return QGradient::RadialGradient;
  if (str == "conical")
    return QGradient::ConicalGradient;
  return QGradient::NoGradient;
}

QGradient::Spread parseGradientSpread(const QString& str)
{
  if (str == "pad")
    return QGradient::PadSpread;
  if (str == "reflect")
    return QGradient::ReflectSpread;
  if (str == "repeat")
    return QGradient::RepeatSpread;
  return QGradient::PadSpread;
}

QGradientStops parseGradientStops(const QJsonArray& jsa)
{
  QGradientStops stops;
  for (const auto& i : jsa) {
    if (!i.isObject()) continue;
    qreal pos = 0;
    const auto js = i.toObject();
    if (const auto v = js["pos"]; v.isDouble())
      pos = std::clamp(v.toDouble(), 0.0, 1.0);
    QColor color = Qt::black;
    if (const auto v = js["color"]; v.isString())
      color = QColor::fromString(v.toString());
    stops.append(QGradientStop(pos, color));
  }
  return stops;
}

QLinearGradient parseLinearGradient(const QJsonObject& js)
{
  QLinearGradient g;
  if (const auto v = js["start"]; v.isObject())
    g.setStart(parsePoint(v.toObject()));
  if (const auto v = js["stop"]; v.isObject())
    g.setFinalStop(parsePoint(v.toObject()));
  return g;
}

QRadialGradient parseRadialGradient(const QJsonObject& js)
{
  QRadialGradient g;
  if (const auto v = js["center"]; v.isObject())
    g.setCenter(parsePoint(v.toObject()));
  if (const auto v = js["focal"]; v.isObject())
    g.setFocalPoint(parsePoint(v.toObject()));
  return g;
}

QConicalGradient parseConicalGradient(const QJsonObject& js)
{
  QConicalGradient g;
  if (const auto v = js["center"]; v.isObject())
    g.setCenter(parsePoint(v.toObject()));
  if (const auto v = js["angle"]; v.isDouble())
    g.setAngle(v.toDouble());
  return g;
}

QGradient parseGradient(const QJsonObject& js)
{
  QGradient::Type gtype = QGradient::NoGradient;
  if (const auto v = js["type"]; v.isString())
    gtype = parseGradientType(v.toString());

  QGradient g(QGradient::PerfectBlue);

  switch (gtype) {
    case QGradient::LinearGradient:
      g = parseLinearGradient(js);
      break;
    case QGradient::RadialGradient:
      g = parseRadialGradient(js);
      break;
    case QGradient::ConicalGradient:
      g = parseConicalGradient(js);
      break;
  }

  if (const auto v = js["stops"]; v.isArray())
    g.setStops(parseGradientStops(v.toArray()));

  if (const auto v = js["spread"]; v.isString())
    g.setSpread(parseGradientSpread(v.toString()));

  g.setCoordinateMode(QGradient::ObjectMode);
  return g;
}

QBrush parseBrush(const QJsonObject& js)
{
  QBrush brush = Qt::NoBrush;
  if (const auto v = js["color"]; v.isString())
    brush = QBrush(QColor::fromString(v.toString()));
  if (const auto v = js["gradient"]; v.isObject())
    brush = QBrush(parseGradient(v.toObject()));
  if (const auto v = js["pattern"]; v.isString())
    brush = QBrush(QPixmap(v.toString()));
  return brush;
}

struct texturing_options {
  void(ClassicSkinBase::*setPerElement)(bool);
  void(ClassicSkinBase::*setStretch)(bool);
  void(ClassicSkinBase::*setBrush)(QBrush);
};

constexpr texturing_options background_options = {
  &ClassicSkinBase::setBackgroundPerElement,
  &ClassicSkinBase::setBackgroundStretch,
  &ClassicSkinBase::setBackground,
};

constexpr texturing_options texture_options = {
  &ClassicSkinBase::setTexturePerElement,
  &ClassicSkinBase::setTextureStretch,
  &ClassicSkinBase::setTexture,
};

void parseTexturingOptions(const QJsonObject& js, const texturing_options& opts, ClassicSkinBase& skin)
{
  if (const auto v = js["per_element"]; v.isBool())
    (skin.*opts.setPerElement)(v.toBool());

  if (const auto v = js["stretch"]; v.isBool())
    (skin.*opts.setStretch)(v.toBool());

  // brush settings are part of item's description
  (skin.*opts.setBrush)(parseBrush(js));
}

void parseClassicSkinBaseParams(const QJsonObject& js, ClassicSkinBase& skin)
{
  if (const auto v = js["vertical"]; v.isBool())
    if (v.toBool()) skin.setOrientation(Qt::Vertical);

  if (const auto v = js["spacing"]; v.isDouble())
    skin.setSpacing(v.toDouble());

  if (const auto v = js["ignore_ax"]; v.isBool())
    skin.setIgnoreAdvanceX(v.toBool());

  if (const auto v = js["ignore_ay"]; v.isBool())
    skin.setIgnoreAdvanceY(v.toBool());

  if (const auto v = js["base_height"]; v.isDouble())
    skin.setGlyphBaseHeight(v.toDouble());

  skin.setBackground(Qt::NoBrush);
  if (const auto v = js["background"]; v.isObject())
    parseTexturingOptions(v.toObject(), background_options, skin);

  skin.setTexture(Qt::NoBrush);
  if (const auto v = js["texture"]; v.isObject())
    parseTexturingOptions(v.toObject(), texture_options, skin);
}

void parseClassicSkinParams(const QJsonObject& js, ClassicSkin& skin)
{
  if (const auto v = js["format"]; v.isString())
    skin.setFormat(v.toString());

  if (const auto v = js["separators"]; v.isString())
    skin.setCustomSeparators(v.toString());

  parseClassicSkinBaseParams(js, skin);
}

template<class EffectType>
std::shared_ptr<Effect> parseTexturingEffect(const QJsonObject& js)
{
  auto effect = std::make_shared<EffectType>();

  if (auto b = parseBrush(js); b != Qt::NoBrush)
    effect->setBrush(std::move(b));

  if (const auto v = js["stretch"]; v.isBool())
    effect->setStretch(v.toBool());

  return effect;
}

std::shared_ptr<Effect> parseEffect(const QJsonObject& js)
{
  auto type_v = js["type"];
  if (!type_v.isString()) return nullptr;

  std::shared_ptr<Effect> effect;

  auto type_s = type_v.toString();
  if (type_s == "new_surface")
    effect = std::make_shared<NewSurfaceEffect>();
  if (type_s == "texture")
    effect = parseTexturingEffect<TexturingEffect>(js);
  if (type_s == "background")
    effect = parseTexturingEffect<BackgroundEffect>(js);

  return effect;
}

} // namespace

class ModernSkin::Impl {
public:
  explicit Impl(const QDir& skin_root)
  {
    _layout = std::make_shared<ModernLayout>();
    init(skin_root);
  }

  ~Impl()
  {
    std::ranges::for_each(std::as_const(_fonts), &QFontDatabase::removeApplicationFont);
  }

  std::shared_ptr<Resource> process(const QDateTime& dt)
  {
    for (const auto& i : std::as_const(_items)) i->process(dt);
    return _layout->resource();
  }

  void setSeparatorAnimationEnabled(bool enabled)
  {
    for (const auto& item : std::as_const(_items))
      item->skin()->setSeparatorAnimationEnabled(enabled);
  }

  void animateSeparator()
  {
    for (const auto& item : std::as_const(_items))
      item->skin()->animateSeparator();
  }

private:
  void parseResources(const QJsonObject& js)
  {
    for (auto iter = js.begin(); iter != js.end(); ++iter) {
      if (iter.value().isString())
        _images[iter.key()] = createResource(_root.absoluteFilePath(iter->toString()));
      if (iter.value().isObject())
        _skins[iter.key()] = parseSkinResources(iter->toObject());
    }
  }

  void parseEffects(const QJsonObject& js)
  {
    for (auto iter = js.begin(); iter != js.end(); ++iter) {
      if (!iter.value().isObject())
        continue;
      if (auto effect = parseEffect(iter->toObject()))
        _effects[iter.key()] = std::move(effect);
    }
  }

  void parseLayout(const QJsonArray& jsa)
  {
    for (const auto& v : jsa) {
      if (!v.isObject())
        continue;
      if (auto item = parseLayoutItem(v.toObject()))
        _layout->addItem(std::move(item));
    }
  }

  std::shared_ptr<LayoutItem> parseLayoutItem(const QJsonObject& js) const
  {
    auto type_v = js["type"];
    if (!type_v.isString()) return nullptr;

    std::shared_ptr<LayoutItem> item;

    auto type_s = type_v.toString();
    if (type_s == "layout")
      item = parseLinearLayout(js);
    if (type_s == "static")
      item = parseStaticItem(js);
    if (type_s == "classic")
      item = parseSkinItem(js);
    if (type_s == "placeholder")
      item = parsePlaceholder(js);

    if (item)
      applyCommonItemOptions(js, *item);

    if (auto sitem = std::dynamic_pointer_cast<SkinItem>(item))
      _items.insert(sitem);

    return item;
  }

  std::shared_ptr<LinearLayout> parseLinearLayout(const QJsonObject& js) const
  {
    auto layout = std::make_shared<LinearLayout>();
    if (const auto v = js["vertical"]; v.isBool())
      if (v.toBool()) layout->setOrientation(Qt::Vertical);

    if (const auto v = js["spacing"]; v.isDouble())
      layout->setSpacing(v.toDouble());

    if (const auto v = js["items"]; v.isArray()) {
      const auto items = v.toArray();
      for (size_t i = 0; i < items.size(); i++) {
        const auto& js_item = items[i];
        if (!js_item.isObject()) continue;
        auto item = parseLayoutItem(js_item.toObject());
        if (!item) continue;
        layout->addItem(std::move(item));
        layout->setItemAlignment(i, parseAlignment(js_item.toObject()));
      }
    }

    if (const auto v = js["ignore_advance"]; v.isBool())
      layout->setIgnoreAdvance(v.toBool());

    layout->updateGeometry();

    return layout;
  }

  std::shared_ptr<LayoutItem> parseStaticItem(const QJsonObject& js) const
  {
    if (const auto v = js["resource"]; v.isString()) {
      auto res = _images.find(v.toString());
      if (res == _images.end() || !*res) return nullptr;
      return std::make_shared<LayoutItem>(*res);
    }

    if (const auto v = js["text"]; v.isString()) {
      QFont font;
      if (const auto v = js["font"]; v.isObject())
        font = parseFont(v.toObject());

      auto factory = std::make_unique<FontResourceFactory>(font);
      auto skin = std::make_unique<StaticText>(std::move(factory));
      skin->setSupportsGlyphBaseHeight(false);
      skin->setIgnoreAdvanceY(true);
      parseClassicSkinBaseParams(js, *skin);
      return std::make_shared<StaticTextItem>(std::move(skin), v.toString());
    }

    return nullptr;
  }

  std::shared_ptr<SkinItem> parseSkinItem(const QJsonObject& js) const
  {
    std::unique_ptr<ClassicSkin> skin;

    if (const auto v = js["resource"]; v.isString())
      skin = parseImageSkin(v.toString());
    if (const auto v = js["font"]; v.isObject())
      skin = parseFontSkin(v.toObject());

    if (!skin) return nullptr;

    parseClassicSkinParams(js, *skin);

    return std::make_shared<SkinItem>(std::move(skin), QDateTime::currentDateTime());
  }

  std::unique_ptr<ClassicSkin> parseImageSkin(const QString& name) const
  {
    auto s_iter = _skins.find(name);
    if (s_iter == _skins.end() || s_iter.value().isEmpty())
      return nullptr;
    auto factory = std::make_unique<ImageResourceFactory>(*s_iter);
    bool supports_separator_animation = factory->supportsSeparatorAnimation();
    auto skin = std::make_unique<ClassicSkin>(std::move(factory));
    skin->setSupportsGlyphBaseHeight(true);
    skin->setSupportsSeparatorAnimation(supports_separator_animation);
    return skin;
  }

  SkinFilesMap parseSkinResources(const QJsonObject& js) const
  {
    SkinFilesMap skin_files;

    auto add_file = [&](QChar ch, QLatin1StringView key) {
      if (const auto v = js[key]; !v.isUndefined())
        skin_files[ch] = parseSkinFile(v);
    };

    for (char c = '0'; c <= '9'; c++) {
      const std::array<char, 2> str_buf = {c, '\0'};
      add_file(c, QLatin1StringView(str_buf.data()));
    }

    add_file(':', QLatin1StringView("s1"));
    add_file(' ', QLatin1StringView("s2"));
    add_file('a', QLatin1StringView("am"));
    add_file('p', QLatin1StringView("pm"));

    return skin_files;
  }

  SkinFileInfo parseSkinFile(const QJsonValue& js) const
  {
    if (js.isString())
      return parseSkinFileImpl(js.toString());
    if (js.isObject())
      return parseSkinFileImpl(js.toObject());
    return {};
  }

  SkinFileInfo parseSkinFileImpl(const QString& fname) const
  {
    return {_root.absoluteFilePath(fname), {}};
  }

  SkinFileInfo parseSkinFileImpl(const QJsonObject& js) const
  {
    QString fname;
    if (const auto v = js["file"]; v.isString())
      fname = v.toString();

    if (fname.isEmpty()) return {};

    GlyphGeometryRaw gg;
    if (const auto v = js["x"]; v.isDouble())
      gg.x = v.toDouble();
    if (const auto v = js["y"]; v.isDouble())
      gg.y = v.toDouble();
    if (const auto v = js["w"]; v.isDouble())
      gg.w = v.toDouble();
    if (const auto v = js["h"]; v.isDouble())
      gg.h = v.toDouble();
    if (const auto v = js["ax"]; v.isDouble())
      gg.ax = v.toDouble();
    if (const auto v = js["ay"]; v.isDouble())
      gg.ay = v.toDouble();

    return {_root.absoluteFilePath(fname), std::move(gg)};
  }

  std::shared_ptr<PlaceholderItem> parsePlaceholder(const QJsonObject& js) const
  {
    QRectF r;

    if (const auto v = js["rect"]; v.isObject())
      r = parseRect(v.toObject());

    if (r.isEmpty()) return nullptr;

    qreal ax = r.width();
    qreal ay = r.height();
    if (const auto v = js["ax"]; v.isDouble())
      ax = v.toDouble();
    if (const auto v = js["ay"]; v.isDouble())
      ay = v.toDouble();

    auto content_v = js["content"];
    if (!content_v.isObject()) return nullptr;

    auto content = parseLayoutItem(content_v.toObject());
    if (!content) return nullptr;

    auto item = std::make_shared<PlaceholderItem>(std::move(r), ax, ay);
    item->setContent(std::move(content));
    item->setContentAlignment(parseAlignment(js));
    item->updateGeometry();
    return item;
  }

  void parseItemsEffects(const QJsonArray& jsa, LayoutItem& item) const
  {
    for (const auto& v : jsa) {
      std::shared_ptr<Effect> effect;

      if (v.isString())
        effect = _effects.value(v.toString());

      if (v.isObject())
        effect = parseEffect(v.toObject());

      if (effect)
        item.decorate(std::move(effect));
    }
  }

  void applyCommonItemOptions(const QJsonObject& js, LayoutItem& item) const
  {
    parseCommonLayoutItemParams(js, item);

    if (const auto v = js["effects"]; v.isArray())
      parseItemsEffects(v.toArray(), item);
  }

  void loadCustomFonts(const QJsonArray& jsa)
  {
    for (const auto& v : jsa) {
      if (!v.isString()) continue;
      auto font_path = _root.absoluteFilePath(v.toString());
      _fonts.insert(QFontDatabase::addApplicationFont(font_path));
    }
  }

  void init(const QDir& skin_root)
  {
    _root = skin_root;

    if (!skin_root.exists("skin.json")) return;

    // change current working directory to skin root
    // during parsing process, this simplifies any file access
    // current working directory will be restored at the end
    PWDChanger _(skin_root.absolutePath());

    QFile skin_cfg(skin_root.absoluteFilePath("skin.json"));
    if (!skin_cfg.open(QIODevice::ReadOnly)) return;

    auto js_obj = QJsonDocument::fromJson(skin_cfg.readAll()).object();
    if (const auto v = js_obj["name"]; v.isString())
      _name = v.toString();

    if (const auto v = js_obj["fonts"]; v.isArray())
      loadCustomFonts(v.toArray());

    if (const auto v = js_obj["resources"]; v.isObject())
      parseResources(v.toObject());

    if (const auto v = js_obj["effects"]; v.isObject())
      parseEffects(v.toObject());

    if (const auto v = js_obj["layout"]; v.isArray())
      parseLayout(v.toArray());
  }

private:
  mutable QSet<std::shared_ptr<SkinItem>> _items;
  std::shared_ptr<ModernLayout> _layout;
  QDir _root;
  QString _name;
  // resources
  QHash<QString, std::shared_ptr<Resource>> _images;
  QHash<QString, SkinFilesMap> _skins;
  // shared effects
  QHash<QString, std::shared_ptr<Effect>> _effects;
  // custom fonts (IDs, for internal use only)
  QSet<int> _fonts;
};


ModernSkin::ModernSkin(const QDir& skin_root)
  : _impl(std::make_unique<Impl>(skin_root))
{}

ModernSkin::~ModernSkin() = default;

std::shared_ptr<Resource> ModernSkin::process(const QDateTime& dt)
{
  return _impl->process(dt);
}

void ModernSkin::setSeparatorAnimationEnabled(bool enabled)
{
  _impl->setSeparatorAnimationEnabled(enabled);
}

void ModernSkin::animateSeparator()
{
  _impl->animateSeparator();
}
