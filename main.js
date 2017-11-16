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
function getOption(prefix) {
    spinLength.value=cppApp.getSettingsValue(prefix+"spinlength",16);
    checkDx.checked=cppApp.getBool(prefix+"checkdx",true);
    checkXx.checked=cppApp.getBool(prefix+"checkxx",true);
    checkSz.checked=cppApp.getBool(prefix+"checksz",true);
    checkTs.checked=cppApp.getBool(prefix+"checkts",true);
}

function setOption(prefix) {
    cppApp.setSettingsValue(prefix+"spinlength",spinLength.value);
    cppApp.setSettingsValue(prefix+"checkdx",checkDx.checked);
    cppApp.setSettingsValue(prefix+"checkxx",checkXx.checked);
    cppApp.setSettingsValue(prefix+"checksz",checkSz.checked);
    cppApp.setSettingsValue(prefix+"checkts",checkTs.checked);
}

function init() {
    usrAndSite.model=cppApp.getStringList("usrandsite");
    usrAndSite.currentIndex=cppApp.getSettingsValue("usindex",0);
    basePass.text=cppApp.getSettingsValue("basepass","");
    userCb.text=cppApp.getSettingsValue("usercur","");
    siteCb.text=cppApp.getSettingsValue("sitecur","");
    
    getOption("");
}

function closing() {
    cppApp.setSettingsValue("usercur",userCb.text);
    cppApp.setSettingsValue("sitecur",siteCb.text);
    cppApp.setSettingsValue("usrandsite",usrAndSite.model);
    cppApp.setSettingsValue("basepass",basePass.text);
    cppApp.setSettingsValue("usindex",usrAndSite.currentIndex);
    
    setOption("");
}

function gen() {
    if(userCb.text===""||basePass.text===""||siteCb.text==="") {
        msgBox.visible=true;
        return;
    }

    var uas=siteCb.text+" "+userCb.text;
    if(usrAndSite.find(uas)===-1) {
        var t=usrAndSite.model;
        t=cppApp.insertString(t,uas);
        usrAndSite.model=t;
    }
    setOption(uas+"/");

    genPass.text=cppApp.genPass(userCb.text,basePass.text,siteCb.text,spinLength.value,
                                checkDx.checked,checkXx.checked,checkSz.checked,checkTs.checked);
    copied.visible=true;
    timerCopied.restart();
}

function timer() {
    copied.visible=false;
}

function delItem(cb) {
    var i=cb.find(cb.editText);
    if(i!==-1) {
        cppApp.removeSettings(cb.editText);
        var k=cb.model;
        k.splice(i,1);
        cb.model=k;
        cb.editText="";
    }
}

function changeus(index) {
    var t=usrAndSite.textAt(index);
    var i=t.indexOf(" ");
    var ckd;
    siteCb.text=t.substr(0,i);
    userCb.text=t.substr(i+1);

    getOption(t+"/");
}
