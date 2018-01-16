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
#ifndef BAIDUPCSSYNC_H
#define BAIDUPCSSYNC_H

#include "pch.h"

class BaiduPcsSync : public QObject
{
    Q_OBJECT
public:
    explicit BaiduPcsSync(QObject *parent = nullptr);
    void download();
    void upload();
    void auth(const QString &authCode);
signals:

public slots:
    void getTokenFinished();
    void refreshTokenFinished();
    void syncError(QNetworkReply::NetworkError err);
    void downloadFinished();
    void uploadFinished();
private:
    void tokenCheck();
    void getNewToken();
    void doJob();
    void refreshToken();
    void refreshFailed();
    void enableMenu();
    QByteArray serializeData();
    bool replyCheck();
    static const QStringList keyList;
    bool syncing;
    enum {
        JOB_DOWNLOAD,
        JOB_UPLOAD
    } job;
    QNetworkAccessManager nm;
    QNetworkReply *reply;
};

#endif // BAIDUPCSSYNC_H
