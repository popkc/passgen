/*
Copyright (c) 2017 popkc (popkcer at gmail dot com)

This program is free software: you can redistribute it and/or modify
t under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CPPAPP_H
#define CPPAPP_H

#include "pch.h"

class BaiduPcsSync;

class CppApp : public QObject
{
    Q_OBJECT
public:
    explicit CppApp(QObject *parent = 0);
    QObject *root;
    QSettings settings;
    BaiduPcsSync *bps;

    Q_INVOKABLE void setSettingsValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant getSettingsValue(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE QVariant getStringList(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE QVariant getBool(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE void removeSettings(const QString &key);
    Q_INVOKABLE const QString genPass(const QString &user, const QString &basepass, const QString &site, int len,
        bool dx, bool xx, bool sz, bool ts);
    Q_INVOKABLE QStringList insertString(QStringList sl, const QString &s);

    Q_INVOKABLE void syncDownload();
    Q_INVOKABLE void syncUpload();
    Q_INVOKABLE void syncAuth(const QString &authCode);

    Q_INVOKABLE void syncImport(bool isImport, const QString &filepath);
signals:
    void needAuth();
    void showMsg(const QString &title, const QString &content);
    void enableMenu();
    void reload();
public slots:
};

extern CppApp *cppApp;

#endif // CPPAPP_H
