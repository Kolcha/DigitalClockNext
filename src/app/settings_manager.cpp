/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_manager.hpp"

#include <QDataStream>
#include <QFile>
#include <QVariantHash>

static const quint32 file_type_magic = 0x44435335;  // DCS5, BE
static const quint16 serialization_format = 0x515A; // QZ, BE

SettingsManagerImpl::SettingsManagerImpl(ApplicationPrivate* app, QObject* parent)
  : SettingsManager(parent)
  , _app(app)
{
}

void SettingsManagerImpl::exportSettings(const QString& filename)
{
  QByteArray buffer;
  QDataStream sout(&buffer, QIODevice::WriteOnly);
  sout.setVersion(QDataStream::Qt_6_5);

  QHash<QString, QVariantHash> settings;
  for (const auto& [tag, ssec] : _app->config_storage()->exportSettings()) {
    settings[tag] = QVariantHash(ssec.begin(), ssec.end());
  }
  sout << settings;

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) return;

  QByteArray sdata = qCompress(buffer, 9);

  QDataStream fout(&file);
  fout.setVersion(QDataStream::Qt_6_5);
  fout << file_type_magic;
  fout << serialization_format;
  fout << qChecksum(sdata);
  fout << static_cast<quint32>(sdata.size());
  fout.writeRawData(sdata.data(), sdata.size());
}

void SettingsManagerImpl::importSettings(const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) return;

  QDataStream fin(&file);
  fin.setVersion(QDataStream::Qt_6_5);

  quint32 magic;
  quint16 dsfmt;
  quint16 dcsum;
  quint32 dsize;

  fin >> magic >> dsfmt >> dcsum >> dsize;

  if (magic != file_type_magic) return;
  if (dsfmt != serialization_format) return;

  QByteArray sdata(dsize, Qt::Initialization{});
  if (fin.readRawData(sdata.data(), sdata.size()) != dsize) return;
  if (qChecksum(sdata) != dcsum) return;

  QByteArray buffer = qUncompress(sdata);
  QDataStream sin(&buffer, QIODevice::ReadOnly);
  sin.setVersion(QDataStream::Qt_6_5);

  QHash<QString, QVariantHash> settings;
  sin >> settings;

  ApplicationPrivate::ConfigStorageType::SettingsData csettings;
  for (auto titer = settings.begin(); titer != settings.end(); ++titer) {
    auto& tsettings = csettings[titer.key()];
    for (auto viter = titer.value().begin(); viter != titer.value().end(); ++viter) {
      tsettings[viter.key()] = viter.value();
    }
  }

  _app->config_storage()->importSettings(csettings);
  _settings_imported = true;
  reconfigure();
}

void SettingsManagerImpl::confirmImport()
{
  if (!_settings_imported) return;
  _app->config_storage()->commitImported();
  _settings_imported = false;
}

void SettingsManagerImpl::discardImport()
{
  if (!_settings_imported) return;
  _app->config_storage()->discardImported();
  _settings_imported = false;
  reconfigure();
}

void SettingsManagerImpl::reconfigure()
{
  std::ranges::for_each(_app->windows(), [this](auto& w) { _app->configureWindow(w.get()); });
}
