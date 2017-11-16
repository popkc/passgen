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
#include "pch.h"
#include "cppapp.h"

CppApp* cppApp;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    cppApp=new CppApp();
    engine.rootContext()->setContextProperty("cppApp",cppApp);
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    
    cppApp->root=engine.rootObjects()[0];

    //auto k=qgetenv("LD_LIBRARY_PATH");
    //qInfo()<<k;
    
    return app.exec();
}
