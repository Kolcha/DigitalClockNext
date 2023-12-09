/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QTest>

#include <any>
#include <memory>
#include <string>
#include <type_traits>

#include "core/settings.hpp"

using ConfigStorageType = ConfigStorage<std::string, std::string, std::any>;
using ConfigClientType = ConfigClient<std::string, std::any>;

static_assert(!std::is_default_constructible_v<ConfigStorageType>, "is_default_constructible");
static_assert(!std::is_copy_constructible_v<ConfigStorageType>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<ConfigStorageType>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<ConfigStorageType>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<ConfigStorageType>, "!is_move_assignable");

template<typename T>
struct toValue<T, std::any> {
  std::any operator ()(const T& v) { return v; }
};

template<typename T>
struct fromValue<std::any, T> {
  T operator ()(const std::any& v) { return std::any_cast<T>(v); }
};

namespace {

using namespace std::string_literals;

class TestConfigBackend final : public ConfigBackend<std::string, std::string, std::any> {
public:
  TestConfigBackend()
    : _all_settings{{
      "app"s, {
        {"ival"s, std::any(42)},
        {"sval"s, std::any("42"s)},
      },
    }}
  {}

  void load(const std::string&) override {}
  void save(const std::string&) override {}

  SettingsData allSettings() const override { return _all_settings; }

  void setValue(const std::string& tag, const std::string& k, const std::any& v) override
  {
    _all_settings[tag][k] = v;
  }

  std::optional<std::any> value(const std::string& tag, const std::string& k) const override
  {
    auto titer = _all_settings.find(tag);
    if (titer == _all_settings.end())
      return std::nullopt;

    auto viter = titer->second.find(k);
    if (viter == titer->second.end())
      return std::nullopt;

    return viter->second;
  }

private:
  SettingsData _all_settings;
};

} // namespace

class SettingsCoreTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void returnDefaultValue();
  void returnSavedValue();
  void simpleSetGet();
  void overwriteValue();
  void testCommit();
  void testDiscard();
  void multipleClients();
  void testExport();
  void testImport();

private:
  std::unique_ptr<ConfigStorageType> _storage;
  std::unique_ptr<ConfigClientType> _config;
};

void SettingsCoreTest::init()
{
  auto backend = std::make_unique<TestConfigBackend>();
  _storage = std::make_unique<ConfigStorageType>(std::move(backend));
  _config = _storage->client("app");
}

void SettingsCoreTest::cleanup()
{
  _config.reset();
  _storage.reset();
}

void SettingsCoreTest::returnDefaultValue()
{
  QCOMPARE(_config->value<int>("ifoo", 32), 32);
  _config = _storage->client("foo_client");
  QCOMPARE(_config->value<int>("ifoo", 32), 32);
}

void SettingsCoreTest::returnSavedValue()
{
  QCOMPARE(_config->value<int>("ival", 32), 42);
}

void SettingsCoreTest::simpleSetGet()
{
  QCOMPARE(_config->value<int>("foo", 32), 32);
  _config->setValue("foo", 64);
  QCOMPARE(_config->value<int>("foo", 32), 64);
}

void SettingsCoreTest::overwriteValue()
{
  QCOMPARE(_config->value<int>("ival", 32), 42);
  _config->setValue("ival", 64);
  QCOMPARE(_config->value<int>("ival", 32), 64);
}

void SettingsCoreTest::testCommit()
{
  QCOMPARE(_config->value<std::string>("sval", ""s), "42"s);
  _config->setValue("sval", "123"s);  // existing value
  _config->setValue("var1", 1);       // new value
  QCOMPARE(_config->value<std::string>("sval", ""s), "123"s);
  QCOMPARE(_config->value<int>("var1", 0), 1);
  _config->commit();
  _config = _storage->client("app");  // same tag must be specified
  QCOMPARE(_config->value<std::string>("sval", ""s), "123"s);
  QCOMPARE(_config->value<int>("var1", 0), 1);
}

void SettingsCoreTest::testDiscard()
{
  QCOMPARE(_config->value<std::string>("sval", ""s), "42"s);
  _config->setValue("sval", "123"s);  // existing value
  _config->setValue("var1", 1);       // new value
  QCOMPARE(_config->value<std::string>("sval", ""s), "123"s);
  QCOMPARE(_config->value<int>("var1", 0), 1);
  _config->discard();
  QCOMPARE(_config->value<std::string>("sval", ""s), "42"s);
  QCOMPARE(_config->value<int>("var1", 0), 0);
}

void SettingsCoreTest::multipleClients()
{
  auto client1 = _storage->client("c1");
  auto client2 = _storage->client("c2");
  // same name variable
  client1->setValue("var", 11);
  client2->setValue("var", 22);
  // unique variables
  client1->setValue("vc1", 10);
  client2->setValue("vc2", 20);
  // commit
  client1->commit();
  client2->commit();
  // re-create clients
  client1 = _storage->client("c1");
  client2 = _storage->client("c2");
  QCOMPARE(client1->value<int>("var", 0), 11);
  QCOMPARE(client2->value<int>("var", 0), 22);
  QCOMPARE(client1->value<int>("vc1", 0), 10);
  QCOMPARE(client2->value<int>("vc2", 0), 20);
  QCOMPARE(client1->value<int>("vc2", 0), 0);
  QCOMPARE(client2->value<int>("vc1", 0), 0);
}

void SettingsCoreTest::testExport()
{
  ConfigStorageType::SettingsData exported;

  // export saved values
  exported = _storage->exportSettings();
  QVERIFY(!exported.empty());

  int value = 32;
  std::string key = "var1"s;
  auto get_int = [](const std::any& v) {
    return fromValue<std::any, int> {}(v);
  };

  // export not commited values
  _config->setValue(key, value);
  exported = _storage->exportSettings();
  QVERIFY(exported["app"s].contains(key));
  QCOMPARE(get_int(exported["app"s][key]), value);

  // export committed values
  _config->commit();
  exported = _storage->exportSettings();
  QVERIFY(exported["app"s].contains(key));
  QCOMPARE(get_int(exported["app"s][key]), value);

  ConfigStorageType::SettingsData imported;
  imported["app"s]["var1"s] = 37;
  imported["app"s]["var2"s] = 84;

  // export imported values
  _storage->importSettings(imported);
  exported = _storage->exportSettings();
  QCOMPARE(get_int(exported["app"s]["var1"s]), 37);
  QCOMPARE(get_int(exported["app"s]["var2"s]), 84);

  // export imported and not commited values
  _config->setValue("var1", 66);
  exported = _storage->exportSettings();
  QCOMPARE(get_int(exported["app"s]["var1"s]), 66);
  QCOMPARE(get_int(exported["app"s]["var2"s]), 84);
}

void SettingsCoreTest::testImport()
{
  std::string key = "var1"s;
  int stored_value = 32;
  int edited_value = 33;
  int imported_value = 37;

  _config->setValue(key, stored_value);
  _config->commit();
  QCOMPARE(_config->value<int>(key, 0), stored_value);

  ConfigStorageType::SettingsData imported;
  imported["app"s][key] = imported_value;

  // import without any changes
  _storage->importSettings(imported);
  QCOMPARE(_config->value<int>(key, 0), imported_value);

  // import with not commited changes
  _config->setValue(key, edited_value);
  QCOMPARE(_config->value<int>(key, 0), edited_value);
  _storage->importSettings(imported);
  QCOMPARE(_config->value<int>(key, 0), imported_value);

  // drop imported settings
  _storage->discardImported();
  QCOMPARE(_config->value<int>(key, 0), stored_value);

  // import-edit-commit
  _storage->importSettings(imported);
  QCOMPARE(_config->value<int>(key, 0), imported_value);
  _config->setValue(key, edited_value);
  QCOMPARE(_config->value<int>(key, 0), edited_value);
  _storage->commitImported();
  QCOMPARE(_config->value<int>(key, 0), edited_value);
  _config->discard();
  QCOMPARE(_config->value<int>(key, 0), imported_value);
}

QTEST_MAIN(SettingsCoreTest)

#include "test_settings_core.moc"
