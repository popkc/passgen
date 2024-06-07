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
#include "baidupcssync.h"
#include "cppapp.h"

const QStringList BaiduPcsSync::keyList(QStringList() << "access_token"
                                                      << "expires_in"
                                                      << "refresh_token"
                                                      << "session_key"
                                                      << "session_secret");

BaiduPcsSync::BaiduPcsSync(QObject *parent)
    : QObject(parent)
    , nm(this)
{
    syncing = false;
}

void BaiduPcsSync::download()
{
    job = JOB_DOWNLOAD;
    tokenCheck();
}

void BaiduPcsSync::upload()
{
    job = JOB_UPLOAD;
    tokenCheck();
}

void BaiduPcsSync::auth(const QString &authCode)
{
    if (authCode.isEmpty()) {
        emit cppApp->showMsg("错误", "授权码不能为空！");
        enableMenu();
        return;
    }

    QNetworkRequest req(QUrl("https://openapi.baidu.com/oauth/2.0/token"));
    QUrlQuery postData;
    postData.addQueryItem("grant_type", "authorization_code");
    postData.addQueryItem("code", authCode);
    postData.addQueryItem("client_id", "e5B4NHFV9hUd50pe3uCGMFyO");
    postData.addQueryItem("client_secret", "95VyImDHFzRzyowuk7q4QVIPbBKGT5Il");
    postData.addQueryItem("redirect_uri", "oob");
    reply = nm.post(req, postData.toString(QUrl::FullyEncoded).toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(reply, &QNetworkReply::finished, this, &BaiduPcsSync::getTokenFinished);
}

void BaiduPcsSync::getTokenFinished()
{
    QJsonParseError err;
    QJsonDocument jd = QJsonDocument::fromJson(reply->readAll(), &err);
    reply->deleteLater();
    if (err.error != QJsonParseError::NoError) {
        emit cppApp->showMsg("get token数据格式错误", err.errorString());
        enableMenu();
        return;
    }

    auto jroot = jd.object();
    if (jroot.contains("error")) {
        emit cppApp->showMsg("get token error", jroot["error"].toString() + "\n" + jroot["error_description"].toString());
        enableMenu();
        return;
    }
    if (jroot["scope"].toString().indexOf("netdisk") == -1) {
        emit cppApp->showMsg("错误", "您未授权本应用使用百度网盘。");
        enableMenu();
        return;
    }

    for (const QString &key : keyList) {
        if (!jroot.contains(key)) {
            emit cppApp->showMsg("get token数据错误", "未找到" + key);
            enableMenu();
            return;
        }

        if (key == "expires_in") {
            uint expires = QDateTime::currentDateTime().toTime_t() + jroot["expires_in"].toInt();
            cppApp->settings.setValue("pcs/expires", expires);
        }
        else
            cppApp->settings.setValue("pcs/" + key, jroot[key].toVariant());
    }

    doJob();
}

void BaiduPcsSync::refreshTokenFinished()
{
    QJsonParseError err;
    QJsonDocument jd = QJsonDocument::fromJson(reply->readAll(), &err);
    reply->deleteLater();
    if (err.error != QJsonParseError::NoError) {
        emit cppApp->showMsg("refresh数据格式错误", err.errorString());
        enableMenu();
        return;
    }

    auto jroot = jd.object();
    if (jroot.contains("error")) {
        refreshFailed();
        return;
    }

    for (const QString &key : keyList) {
        if (!jroot.contains(key)) {
            refreshFailed();
            return;
        }

        if (key == "expires_in") {
            uint expires = QDateTime::currentDateTime().toTime_t() + jroot["expires_in"].toInt();
            cppApp->settings.setValue("pcs/expires", expires);
        }
        else
            cppApp->settings.setValue("pcs/" + key, jroot[key].toVariant());
    }

    doJob();
}

void BaiduPcsSync::syncError(QNetworkReply::NetworkError err)
{
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid() && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
        getNewToken();
    }
    else {
        cppApp->showMsg("同步失败", QString::number(err) + "\n" + reply->readAll());
        enableMenu();
    }
    reply->deleteLater();
    disconnect(reply, 0, 0, 0);
}

void BaiduPcsSync::downloadFinished()
{
    if (!replyCheck())
        return;
}

void BaiduPcsSync::uploadFinished()
{
    if (!replyCheck())
        return;

    enableMenu();
    cppApp->showMsg("成功", "上传完成！");
}

void BaiduPcsSync::tokenCheck()
{
    if (syncing)
        return;
    syncing = true;
    uint etime = cppApp->settings.value("pcs/expires", 0).toUInt();
    if (QDateTime::currentDateTime().toTime_t() + 10 >= etime) {
        QString rt = cppApp->settings.value("pcs/refresh_token").toString();
        if (rt.isEmpty()) {
            getNewToken();
        }
        else {
            refreshToken();
        }
    }
    else
        doJob();
}

void BaiduPcsSync::getNewToken()
{
    QDesktopServices::openUrl(QUrl("http://openapi.baidu.com/oauth/2.0/authorize?client_id=e5B4NHFV9hUd50pe3uCGMFyO&response_type=code&redirect_uri=oob&scope=netdisk"));
    emit cppApp->needAuth();
}

void BaiduPcsSync::doJob()
{
    // emit cppApp->showMsg("成功","获得token");
    if (job == JOB_DOWNLOAD) {
        QNetworkRequest req(QUrl("https://d.pcs.baidu.com/rest/2.0/pcs/file?method=download&path=%2fpassgen&access_token=" + cppApp->settings.value("pcs/access_token").toString()));
        reply = nm.get(req);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(syncError(QNetworkReply::NetworkError)));
        connect(reply, &QNetworkReply::finished, this, &BaiduPcsSync::downloadFinished);
    }
    else if (job == JOB_UPLOAD) {
        QNetworkRequest req(QUrl("https://pcs.baidu.com/rest/2.0/pcs/file?method=upload&path=%2fpassgen&ondup=overwrite&access_token=" + cppApp->settings.value("pcs/access_token").toString()));
        QHttpMultiPart *mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart hp;
        hp.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        hp.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\""));
        hp.setBody(serializeData());
        mp->append(hp);

        reply = nm.post(req, mp);
        mp->setParent(reply);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(syncError(QNetworkReply::NetworkError)));
        connect(reply, &QNetworkReply::finished, this, &BaiduPcsSync::uploadFinished);
    }
}

void BaiduPcsSync::refreshToken()
{
    QNetworkRequest req(QUrl("https://openapi.baidu.com/oauth/2.0/token"));
    QUrlQuery postData;
    postData.addQueryItem("grant_type", "refresh_token");
    postData.addQueryItem("refresh_token", cppApp->settings.value("pcs/refresh_token").toString());
    postData.addQueryItem("client_id", "e5B4NHFV9hUd50pe3uCGMFyO");
    postData.addQueryItem("client_secret", "95VyImDHFzRzyowuk7q4QVIPbBKGT5Il");
    reply = nm.post(req, postData.toString(QUrl::FullyEncoded).toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    connect(reply, &QNetworkReply::finished, this, &BaiduPcsSync::refreshTokenFinished);
}

void BaiduPcsSync::refreshFailed()
{
    cppApp->settings.remove("pcs/refresh_token");
    getNewToken();
}

void BaiduPcsSync::enableMenu()
{
    emit cppApp->enableMenu();
}

QByteArray BaiduPcsSync::serializeData()
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    QStringList sl = cppApp->settings.value("usrandsite").toStringList();
    ds << sl;
    for (const QString &s : sl) {
        ds << cppApp->settings.value(s + "/spinlength", 16).toUInt()
           << cppApp->settings.value(s + "/checkdx", true).toBool()
           << cppApp->settings.value(s + "/checkxx", true).toBool()
           << cppApp->settings.value(s + "/checksz", true).toBool()
           << cppApp->settings.value(s + "/checkts", true).toBool();
    }
    ba = qCompress(ba);
    return ba;
}

bool BaiduPcsSync::replyCheck()
{
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()) {
        int hs = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (hs == 401)
            getNewToken();
        else if (hs != 200) {
            cppApp->showMsg("同步失败", QString::number(hs) + "\n" + reply->readAll());
            enableMenu();
        }
        reply->deleteLater();
        return false;
    }
    return true;
}
