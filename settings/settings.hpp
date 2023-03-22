#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

// TODO: consider namespace for that
// specializations must be provided
template<typename T, typename Value>
struct toValue {
  Value operator ()(const T& v);
};

template<typename Value, typename T>
struct fromValue {
  T operator ()(const Value& v);
};

// public (end-user) interface
// Key - string-like type
// Value - any type container type
template<typename Key, typename Value>
class ConfigClient {
public:
  virtual ~ConfigClient() = default;

  virtual void commit() = 0;
  virtual void discard() = 0;

  template<typename T>
  void setValue(const Key& k, const T& v)
  {
    setValue(k, toValue<T, Value>{}(v));
  }

  template<typename T>
  T value(const Key& k, const T& def) const
  {
    auto val = getValue(k);
    return val ? fromValue<Value, T>{}(*val) : def;
  }

protected:
  virtual void setValue(const Key& k, Value&& v) = 0;
  virtual std::optional<Value> getValue(const Key& k) const = 0;
};


// setting storage backend (aka serialization/deserialization)
// QSettings, JSON, and so on
template<typename Tag, typename Key, typename Value>
class ConfigBackend {
public:
  virtual ~ConfigBackend() = default;

  virtual void load(const Tag& tag) = 0;
  virtual void save(const Tag& tag) = 0;

  using SettingsMap = std::unordered_map<Key, Value>;
  using SettingsData = std::unordered_map<Tag, SettingsMap>;
  virtual SettingsData allSettings() const = 0;

  virtual void setValue(const Tag& tag, const Key& k, const Value& v) = 0;
  virtual std::optional<Value> value(const Tag& tag, const Key& k) const = 0;
};


// TODO: add concepts
template<typename Map>
void merge_with_override(Map& target, const Map& source)
{
  for (const auto& [key, value] : source)
    target[key] = value;
}


// Tag - string-like type
template<typename Tag, typename Key, typename Value>
class ConfigStorage final {
  class ConfigClientImpl final : public ConfigClient<Key, Value> {
    using ConfigStorageType = ConfigStorage<Tag, Key, Value>;

  public:
    ConfigClientImpl(ConfigStorageType* storage, Tag&& tag) noexcept
      : _storage(storage)
      , _tag(std::forward<Tag>(tag))
    {}

    ConfigClientImpl(const ConfigClientImpl&) = delete;
    ConfigClientImpl(ConfigClientImpl&&) = default;

    ConfigClientImpl& operator=(const ConfigClientImpl&) = delete;
    ConfigClientImpl& operator=(ConfigClientImpl&&) = default;

    inline void commit() override { _storage->commit(_tag); }
    inline void discard() override { _storage->discard(_tag); }

  protected:
    inline void setValue(const Key& k, Value&& v) override
    {
      _storage->setValue(_tag, k, std::forward<Value>(v));
    }

    inline std::optional<Value> getValue(const Key& k) const override
    {
      return _storage->value(_tag, k);
    }

  private:
    ConfigStorageType* _storage;
    Tag _tag;
  };

  using ConfigBackendType = ConfigBackend<Tag, Key, Value>;

public:
  explicit ConfigStorage(std::unique_ptr<ConfigBackendType> backend) noexcept
    : _backend(std::move(backend))
  {}

  ConfigStorage(const ConfigStorage&) = delete;
  ConfigStorage(ConfigStorage&&) = default;

  ConfigStorage& operator=(const ConfigStorage&) = delete;
  ConfigStorage& operator=(ConfigStorage&&) = default;

  using SettingsData = ConfigBackendType::SettingsData;

  SettingsData exportSettings() const
  {
    auto data = _backend->allSettings();

    for (const auto& [tag, settings] : _import_cache)
      merge_with_override(data[tag], settings);

    for (const auto& [tag, settings] : _current_cache)
      merge_with_override(data[tag], settings);

    return data;
  }

  void importSettings(SettingsData data) noexcept
  {
    _current_cache.clear();
    _import_cache = std::move(data);
  }

  void commitImported()
  {
    for (const auto& [tag, settings] : _import_cache) {
      for (const auto& [key, value] : settings)
        _backend->setValue(tag, key, value);
      _backend->save(tag);
    }
    _import_cache.clear();
  }

  void discardImported() noexcept
  {
    _import_cache.clear();
  }

  // function must be NON-const because non-const this is required
  std::unique_ptr<ConfigClient<Key, Value>> client(Tag&& tag)
  {
    return std::make_unique<ConfigClientImpl>(this, std::forward<Tag>(tag));
  }

protected:
  void commit(const Tag& tag)
  {
    // it's fine to create an empty container
    auto& current = _current_cache[tag];
    auto& imported = _import_cache[tag];
    if (!imported.empty()) {
      merge_with_override(imported, current);
    } else {
      for (const auto& [k, v] : current)
        _backend->setValue(tag, k, v);
      _backend->save(tag);
    }
    current.clear();
  }

  void discard(const Tag& tag)
  {
    _current_cache.erase(tag);
  }

  void setValue(const Tag& tag, const Key& key, Value&& value)
  {
    _current_cache[tag][key] = std::forward<Value>(value);
  }

  std::optional<Value> value(const Tag& tag, const Key& key) const
  {
    if (auto val = find_value(_current_cache, tag, key))
      return val;

    if (auto val = find_value(_import_cache, tag, key))
      return val;

    _backend->load(tag);
    return _backend->value(tag, key);
  }

private:
  // it should be free function, but as so as it is too specfic to this class
  // it was considered to make it private static member
  static std::optional<Value> find_value(const SettingsData& data, const Tag& tag, const Key& key)
  {
    auto titer = data.find(tag);
    if (titer == data.end())
      return std::nullopt;

    const auto& settings = titer->second;
    auto viter = settings.find(key);
    if (viter == settings.end())
      return std::nullopt;

    return viter->second;
  }

private:
  std::unique_ptr<ConfigBackendType> _backend;
  SettingsData _current_cache;
  SettingsData _import_cache;
};
