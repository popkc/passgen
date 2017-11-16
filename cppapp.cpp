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
#include "cppapp.h"

CppApp::CppApp(QObject *parent) : QObject(parent),
    settings("popkc","passgen")
{
    
}

void CppApp::setSettingsValue(const QString &key, const QVariant &value)
{
    settings.setValue(key,value);
}

QVariant CppApp::getSettingsValue(const QString &key, const QVariant &defaultValue)
{
    return settings.value(key,defaultValue);
}

QVariant CppApp::getStringList(const QString &key, const QVariant &defaultValue)
{
    return settings.value(key,defaultValue).toStringList();
}

QVariant CppApp::getBool(const QString &key, const QVariant &defaultValue)
{
    return settings.value(key,defaultValue).toBool();
}

void CppApp::removeSettings(const QString &key)
{
    settings.remove(key);
}

const QString CppApp::genPass(const QString &user, const QString &bpass, const QString &ss,int len,
                              bool dx,bool xx,bool sz,bool ts)
{
    QByteArray usr=user.toUtf8();
    QByteArray basepass=bpass.toUtf8();
    QByteArray site=ss.toUtf8();

    usr=QCryptographicHash::hash(usr,QCryptographicHash::Md5);
    basepass=QCryptographicHash::hash(basepass,QCryptographicHash::Md5);
    site=QCryptographicHash::hash(site,QCryptographicHash::Md5);
    usr=QCryptographicHash::hash(usr+basepass+site,QCryptographicHash::Sha512);
    QByteArray avai,bdx,bxx,bsz,bts;
    if(dx) {
        bdx.resize(26);
        for(char c=0;c<26;c++) {
            bdx[c]=c+'A';
        }
    }
    if(xx) {
        bxx.resize(26);
        for(char c=0;c<26;c++) {
            bxx[c]=c+'a';
        }
    }
    if(sz) {
        bsz.resize(10);
        for(char c=0;c<10;c++) {
            bsz[c]=c+'0';
        }
    }
    if(ts) {
        bts.resize(32);
        for(char c=0;c<15;c++) {
            bts[c]=c+33;
        }
        for(char c=0;c<7;c++) {
            bts[15+c]=c+58;
        }
        for(char c=0;c<6;c++) {
            bts[22+c]=c+91;
        }
        for(char c=0;c<4;c++) {
            bts[28+c]=c+123;
        }
    }
    
    avai=bdx+bxx+bsz+bts;
    basepass.resize(len);
    int pos=0;
    if(len>4) {
        if(dx) {
            basepass[pos]=bdx[ static_cast<unsigned char>(usr[pos]) % bdx.length() ];
            pos++;
        }
        if(xx) {
            basepass[pos]=bxx[ static_cast<unsigned char>(usr[pos]) % bxx.length() ];
            pos++;
        }
        if(sz) {
            basepass[pos]=bsz[ static_cast<unsigned char>(usr[pos]) % bsz.length() ];
            pos++;
        }
        if(ts) {
            basepass[pos]=bts[ static_cast<unsigned char>(usr[pos]) % bts.length() ];
            pos++;
        }
    }
    for(;pos<len;pos++) {
        basepass[pos]=avai[ static_cast<unsigned char>(usr[pos]) % avai.length() ];
    }
    
    QClipboard *cb=QGuiApplication::clipboard();
    cb->setText(basepass);
    return basepass;
}

QStringList CppApp::insertString(QStringList sl, const QString &s)
{
    sl.append(s);
    sl.sort();
    return sl;
}
