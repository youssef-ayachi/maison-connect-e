function data() {
var url = "http://www.domaine.com/arduino.json";
var data = UrlFetchApp.fetch(url);
var json = data.getContentText();
var data = JSON.parse(json);
var lum=data.l;
var ven=data.v;
var chauf=data.c;
var temp=data.t;
var hum=data.hu;
var mou=data.m;
var heure=data.h;
var stat = SpreadsheetApp.getActiveSpreadsheet().getSheets()[0];
var last3 = stat.getLastRow()+1;
var range = stat.getRange("A1:G" + last3);
var d = new Date();
var h = d.getHours();
var n = d.getMinutes();
var heurp=h+":"+n;
var dif = heure.split(":");
if((h-dif[0]<2 && n-dif[1]<30) || (temp>range.getCell(last3-1, 5).getValue() || hum > range.getCell(last3-1, 6).getValue())){
range.getCell(last3, 1).setValue(d);
range.getCell(last3, 2).setValue(lum);
range.getCell(last3, 3).setValue(ven);
range.getCell(last3, 4).setValue(chauf);
range.getCell(last3, 5).setValue(temp);
range.getCell(last3, 6).setValue(hum);
range.getCell(last3, 7).setValue(mou);
}

if (temp>30){ ////send command to Arduino when temperature is high than 30 c. In this case, arduino turn motor to the left
var html = "http://www.domaine.com/arduinophp.php?select=mch,";
UrlFetchApp.fetch(html+20);
}else if (temp<30){ /////send command to Arduino when temperature is less than 30 c, In this case, arduino turn motor to the rigth
var html = "http://www.domaine.com/arduinophp.php?select=mch,";
UrlFetchApp.fetch(html+32);
}
}
