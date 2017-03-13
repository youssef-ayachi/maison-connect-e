#include "EtherCard.h"
#include "ArduinoJson.h"
#include "string.h"
#include "DHT.h"

#define DHTTYPE DHT11
DHT dht(9, DHTTYPE);
//ethernet interface mac address, must be unique on the LAN
static byte mymac[] = {0x74,0xDE,0x2B,0x83,0x41,0x79};
static byte myip[] = { 192,168,1,44};
byte Ethernet::buffer[500];
BufferFiller bfill;
static uint32_t timer;
static uint32_t timers;
const char website[] PROGMEM = "www.domaine.com";
String retour ="";
int emp=0;
char json[80];
int state=0;
int con=0;

static void my_callback (byte status, word off, word len) {
Ethernet::buffer[off+300] = 0;
Serial.println("Contenu HTML");
String tout = (const char*) Ethernet::buffer + off;
int pos = tout.indexOf("plain");
String var = tout.substring(pos+9);
retour=var;
int rotation;
int posi = var.indexOf(",");
if(retour.indexOf(",")>0){
retour = var.substring(0, posi);
rotation = var.substring(posi+1).toInt();
}
Serial.println(tout);
Serial.println("La variable de commande");
Serial.println(var);
Serial.println(rotation);
if (retour == "alc"){
digitalWrite(7, HIGH);
emp=0;
} else if(retour == "elc"){
digitalWrite(7, LOW);
emp=0;
} else if (retour == "dv"){
digitalWrite(6, HIGH);
emp=0;
} else if(retour == "av"){
digitalWrite(6, LOW);
emp=0;
} else if (retour == "mch" && emp==0){
digitalWrite(10, HIGH);
delay(rotation*100);
digitalWrite(10, LOW);
emp=1;
} else if(retour == "ach"){
emp=0;
digitalWrite(10, LOW);
}
state=1;
con=0;
}

void setup () {
Serial.begin(9600);
dht.begin();
pinMode(7,OUTPUT);
pinMode(6,OUTPUT);
pinMode(10,OUTPUT);
pinMode(2,OUTPUT);
digitalWrite(2, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(10, LOW);
Serial.println(F("n[webClient]"));
if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
Serial.println(F("Failed to access Ethernet controller"));
if (!ether.dhcpSetup())
Serial.println(F("DHCP failed"));
ether.printIp("IP: ", ether.myip);
ether.printIp("GW: ", ether.gwip); 
ether.printIp("DNS: ", ether.dnsip);
if (!ether.dnsLookup(website))
Serial.println("DNS failed");
ether.printIp("SRV: ", ether.hisip);
if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
Serial.println( "Failed to access Ethernet controller");
ether.staticSetup(myip);
con=0;
}

void loop () {
float h = dht.readHumidity();
float t = dht.readTemperature();
float f = dht.readTemperature(true);
float hic = dht.computeHeatIndex(t, h, false);
humidites = h;
degre =t;
if (isnan(h) || isnan(t) || isnan(f)) {
Serial.println("Failed to read from DHT sensor!");
return;
}
int degree=0;
int chaleur=0;
int lampe=0;
degree = digitalRead(7);
chaleur = digitalRead(6);
lampe = digitalRead(10);
if(emp==1){lampe=1;}
///////////////////json//////////////////////////
StaticJsonBuffer<80> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
root["l"] = degree;
root["v"] = chaleur;
root["c"] = lampe;
root["t"] = t;
root["hu"] = h;
root["m"] = hic;
root["h"] = "H-R";
root.printTo(json,sizeof(json));
///////////////////////////////////////////////
ether.packetLoop(ether.packetReceive());
if (millis() > timer) {
state=0;
timer = millis() + 5000;
ether.browseUrl(PSTR("/commands.txt"), "", website, my_callback);
con +=1;
}
if (state==1 && millis() > timers){
timers = millis() + 7000;
ether.browseUrl(PSTR("/arduino.php?contenu="), (char*)json, website, NULL);
state=0;
}

if(con>9){
digitalWrite(2, HIGH);
delay(100);
digitalWrite(2, LOW);
software_Reset();
}
word len = ether.packetReceive();
word pos = ether.packetLoop(len);
if (pos)
ether.httpServerReply(homePage());
}

void software_Reset()
{
asm volatile (" jmp 0"); 
}
