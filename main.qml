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
import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import "main.js" as Logic

ApplicationWindow {
    visible: true
    width: 400
    height: 300
    title: qsTr("统一密码生成器")

    Flickable {
        anchors.fill: parent;
        anchors.margins: 5;
        contentHeight: content.height;
        contentWidth: width;
        Column {
            id:content;
            spacing: 5;
            anchors.left: parent.left;
            anchors.right: parent.right;
            RowLayout {
                width: parent.width;
                ComboBox {
                    Layout.fillWidth: true;
                    id: usrAndSite;
                    onActivated: Logic.changeus(index);
                }
                Button {
                    text: "删除";
                    onClicked: Logic.delItem(usrAndSite);
                }
            }

            RowLayout {
                width: parent.width;
                Text {
                    text:"用户名："
                }
                TextField {
                    id: userCb;
                    Layout.fillWidth: true;
                }
            }
            RowLayout {
                width: parent.width;
                Text {
                    text:"基础密码：";
                }
                TextField {
                    Layout.fillWidth: true;
                    echoMode: TextInput.PasswordEchoOnEdit;
                    id:basePass;
                }
            }
            RowLayout {
                width: parent.width;
                Text {
                    text: "网站："
                }
                TextField {
                    Layout.fillWidth: true;
                    id:siteCb;
                }
            }
            Row {
                width: parent.width;
                Text {
                    text: "长度："
                }
                SpinBox {
                    minimumValue: 1;
                    maximumValue: 64;
                    id: spinLength;
                }
            }
            Row {
                width: parent.width;
                CheckBox {
                    text: "大写"
                    id:checkDx;
                }
                CheckBox {
                    text:"小写";
                    id:checkXx;
                }
                CheckBox {
                    text:"数字";
                    id:checkSz;
                }
                CheckBox {
                    text:"特殊符号";
                    id:checkTs;
                }
            }

            RowLayout {
                width: parent.width;
                Button {
                    text:"生成"
                    Layout.fillWidth: true;
                    onClicked: Logic.gen();
                }
                Text {
                    text:"已复制";
                    visible: false;
                    id:copied;
                }
            }
            RowLayout {
                width: parent.width;
                Text {
                    text:"生成的密码：";
                }
                TextField {
                    Layout.fillWidth: true;
                    id:genPass;
                }
            }
        }
    }
    
    MessageDialog {
        text: "不能为空。";
        title: "错误";
        id:msgBox;
    }
    Timer {
        interval: 3000;
        onTriggered: Logic.timer();
        id:timerCopied;
    }
    
    Component.onCompleted: Logic.init();
    onClosing: Logic.closing();
}
