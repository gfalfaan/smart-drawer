#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <Preferences.h>

#include "web_index.h"
#include "led_control.h"
#include "komunikasi_atmega.h"
#include "komunikasi_arduinonano.h"  
#include "time.h"             // Library bawaan untuk ambil waktu internet
#include "jadwalsholat.h"     // Masukkan tab jadwal sholat yang baru dibuat

// Daftar Nama Effect Bawaan WS2812FX
const char FX_NAMES_JSON[] PROGMEM = "[\"Solid\",\"Blink\",\"Breathe\",\"Color Wipe\",\"Color Wipe Random\",\"Random Color\",\"Single Dynamic\",\"Multi Dynamic\",\"Rainbow\",\"Rainbow Cycle\",\"Scan\",\"Dual Scan\",\"Fade\",\"Theater Chase\",\"Theater Chase Rainbow\",\"Running Lights\",\"Twinkle\",\"Twinkle Random\",\"Twinkle Fade\",\"Twinkle Fade Random\",\"Sparkle\",\"Flash Sparkle\",\"Hyper Sparkle\",\"Strobe\",\"Strobe Rainbow\",\"Multi Strobe\",\"Blink Rainbow\",\"Chase White\",\"Chase Color\",\"Chase Random\",\"Chase Rainbow\",\"Chase Flash\",\"Chase Flash Random\",\"Chase Rainbow White\",\"Colorful\",\"Traffic Light\",\"Color Sweep Random\",\"Running Color\",\"Running Red Blue\",\"Running Random\",\"Larson Scanner\",\"Comet\",\"Fireworks\",\"Fireworks Random\",\"Merry Christmas\",\"Fire Flicker\",\"Fire Flicker Soft\",\"Fire Flicker Intense\",\"Circus Combustus\",\"Halloween\",\"Bicolor Chase\",\"Tricolor Chase\",\"ICU\"]";

// Variabel Timer Jadwal Sholat
unsigned long lastJadwalUpdate = 0;
const unsigned long JADWAL_INTERVAL = 24UL * 60UL * 60UL * 1000UL; // 24 Jam
bool jadwalPertama = true;

AsyncWebServer server(80);
Preferences    prefs;

#define DATA_FILE "/data.json"
#define CFG_NS    "sdrawer"

struct Config {
  char     ssid[64]    = "";
  char     pass[64]    = "";
  char     apSSID[32]  = "SmartDrawer";
  char     apPass[32]  = "smartdrawer";
  uint16_t ledCount    = 25;
  uint8_t  ledPin      = 14;
  char     devName[32] = "smartdrawer";
} cfg;

void loadConfig() {
  prefs.begin(CFG_NS, true);
  prefs.getString("ssid",   cfg.ssid,   sizeof(cfg.ssid));
  prefs.getString("pass",   cfg.pass,   sizeof(cfg.pass));
  prefs.getString("apSSID", cfg.apSSID, sizeof(cfg.apSSID));
  prefs.getString("apPass", cfg.apPass, sizeof(cfg.apPass));
  prefs.getString("devName",cfg.devName,sizeof(cfg.devName));
  cfg.ledCount = prefs.getUShort("ledCnt", 25);
  cfg.ledPin   = prefs.getUChar("ledPin",  14);
  prefs.end();
  
  if (strlen(cfg.devName) == 0) strlcpy(cfg.devName, "smartdrawer", sizeof(cfg.devName));
}

void saveConfig() {
  prefs.begin(CFG_NS, false);
  prefs.putString("ssid",    cfg.ssid);
  prefs.putString("pass",    cfg.pass);
  prefs.putString("apSSID",  cfg.apSSID);
  prefs.putString("apPass",  cfg.apPass);
  prefs.putString("devName", cfg.devName);
  prefs.putUShort("ledCnt",  cfg.ledCount);
  prefs.putUChar("ledPin",   cfg.ledPin);
  prefs.end();
}

void sendProgmemJson(AsyncWebServerRequest *req, const char* pgmStr) {
  AsyncWebServerResponse *response = req->beginChunkedResponse("application/json", [pgmStr](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
    size_t len = strlen_P(pgmStr);
    if (index >= len) return 0;
    size_t remaining = len - index;
    size_t chunkLen = (remaining > maxLen) ? maxLen : remaining;
    memcpy_P(buffer, pgmStr + index, chunkLen);
    return chunkLen;
  });
  response->addHeader("Access-Control-Allow-Origin", "*");
  req->send(response);
}

// KEMBALIKAN FUNGSI CONFIG UNTUK MENU SETTINGS WEB
String configToJson() {
  String j = "{";
  j += "\"ssid\":\""      + String(cfg.ssid)    + "\",";
  j += "\"apSSID\":\""    + String(cfg.apSSID)  + "\",";
  j += "\"apPass\":\""    + String(cfg.apPass)  + "\",";
  j += "\"devName\":\""   + String(cfg.devName) + "\",";
  j += "\"hostname\":\""  + String(cfg.devName) + ".local\",";
  j += "\"ledCount\":"    + String(cfg.ledCount) + ",";
  j += "\"ledPin\":"      + String(cfg.ledPin)   + ",";
  j += "\"staIP\":\""     + (WiFi.status()==WL_CONNECTED ? WiFi.localIP().toString() : "") + "\",";
  j += "\"apIP\":\""      + WiFi.softAPIP().toString() + "\",";
  j += "\"staOK\":"       + String(WiFi.status()==WL_CONNECTED?"true":"false") + ",";
  j += "\"apActive\":true,";
  j += "\"rssi\":"        + String(WiFi.RSSI()) + ",";
  j += "\"freeHeap\":"    + String(ESP.getFreeHeap()) + ",";
  j += "\"chipModel\":\"" + String(ESP.getChipModel()) + "\",";
  j += "\"flashMB\":"     + String(ESP.getFlashChipSize()/1024/1024);
  j += "}";
  return j;
}

void setupRoutes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) { req->send_P(200, "text/html", index_html); });
  server.on("/json/eff", HTTP_GET, [](AsyncWebServerRequest *req) { sendProgmemJson(req, FX_NAMES_JSON); });
  server.on("/json/state", HTTP_GET, [](AsyncWebServerRequest *req) { req->send(200, "application/json", buildStateJson()); });
// Jam Digital 
server.on("/api/rtc", HTTP_GET, [](AsyncWebServerRequest *req) {
  String json = "{";
  json += "\"time\":\"" + rtcTime + "\",";
  json += "\"alarm\":\"" + rtcAlarm + "\",";
  json += "\"alarmon\":" + String(rtcAlarmOn);
  json += "}";
  req->send(200, "application/json", json);
});
server.on("/api/settime", HTTP_POST, [](AsyncWebServerRequest *req) {
  if (req->hasParam("t", true)) {
    String t = req->getParam("t", true)->value(); // HH:MM
    Serial2.println("SETTIME:" + t + ":00");
    String cmd = "SETTIME:" + t + ":00";
    Serial.println(cmd);      // debug ke monitor
    Serial2.println(cmd);     // kirim ke ATmega
    req->send(200, "application/json", "{\"ok\":true}");
  } else req->send(400);
});
server.on("/api/setalarm", HTTP_POST, [](AsyncWebServerRequest *req) {
  if (req->hasParam("a", true)) {
    String a = req->getParam("a", true)->value(); // format HH:MM
    a.trim(); // 🔥 buang karakter aneh
    // validasi panjang
    if (a.length() != 5 || a.charAt(2) != ':') {
      Serial.println("FORMAT SALAH");
      req->send(400, "application/json", "{\"error\":\"format\"}");
      return;
    }
    int h = a.substring(0,2).toInt();
    int m = a.substring(3,5).toInt();
    // validasi jam
    if (h < 0 || h > 23 || m < 0 || m > 59) {
      Serial.println("WAKTU INVALID");
      req->send(400, "application/json", "{\"error\":\"range\"}");
      return;
    }
    // 🔥 format ulang (INI KUNCI)
    char cmd[20];
    sprintf(cmd, "SETALARM:%02d:%02d", h, m);
    Serial.println(cmd);      // debug
    Serial2.println(cmd);     // kirim ke ATmega
    req->send(200, "application/json", "{\"ok\":true}");
  } else {
    req->send(400, "application/json", "{\"error\":\"no_param\"}");
  }
});
server.on("/api/setalarmon", HTTP_POST, [](AsyncWebServerRequest *req) {

  if (req->hasParam("on", true)) {

    String onStr = req->getParam("on", true)->value();
    onStr.trim();

    Serial.println("KIRIM: SETALARMON:" + onStr);   // 🔥 DEBUG DI SINI
    Serial2.println("SETALARMON:" + onStr);         // kirim ke ATmega

    req->send(200, "application/json", "{\"ok\":true}");

  } else {
    req->send(400);
  }
});
server.on("/api/setmode", HTTP_POST, [](AsyncWebServerRequest *req) {
  if (req->hasParam("m", true)) {
    String m = req->getParam("m", true)->value();
    Serial2.println("SETMODE:" + m);
    req->send(200, "application/json", "{\"ok\":true}");
  } else req->send(400);
});
server.on("/api/settone", HTTP_POST, [](AsyncWebServerRequest *req) {
  if (req->hasParam("t", true)) {
    String t = req->getParam("t", true)->value();
    Serial2.println("SETTONE:" + t);
    req->send(200, "application/json", "{\"ok\":true}");
  } else req->send(400);
}); 
//Jam Digital

  server.addHandler(new AsyncCallbackJsonWebHandler("/json/state", [](AsyncWebServerRequest *req, JsonVariant &json) {
    if (json.is<JsonObject>()) handleLED(json.as<JsonObject>());
    req->send(200, "application/json", "{\"success\":true}");
  }));

  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *req) { req->send(LittleFS, DATA_FILE, "application/json"); });
  server.addHandler(new AsyncCallbackJsonWebHandler("/save", [](AsyncWebServerRequest *req, JsonVariant &json) {
    File f = LittleFS.open(DATA_FILE, "w");
    serializeJson(json, f);
    f.close();
    req->send(200, "application/json", "{\"success\":true}");
  }));

  // --- KEMBALIKAN ENDPOINT SETTINGS YANG HILANG ---
  server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(200, "application/json", configToJson());
  });

  server.addHandler(new AsyncCallbackJsonWebHandler("/api/config/system", [](AsyncWebServerRequest *req, JsonVariant &json) {
    if (json.is<JsonObject>()) {
      JsonObject j = json.as<JsonObject>();
      if (j.containsKey("devName")) strlcpy(cfg.devName, j["devName"], sizeof(cfg.devName));
      if (j.containsKey("ledCount")) cfg.ledCount = j["ledCount"].as<uint16_t>();
      if (j.containsKey("ledPin"))   cfg.ledPin   = j["ledPin"].as<uint8_t>();
      saveConfig();
      req->send(200, "application/json", "{\"success\":true}");
    } else { req->send(400); }
  }));

  server.addHandler(new AsyncCallbackJsonWebHandler("/api/config/wifi", [](AsyncWebServerRequest *req, JsonVariant &json) {
    if (json.is<JsonObject>()) {
      JsonObject j = json.as<JsonObject>();
      if (j.containsKey("ssid"))   strlcpy(cfg.ssid,   j["ssid"],   sizeof(cfg.ssid));
      if (j.containsKey("pass"))   strlcpy(cfg.pass,   j["pass"],   sizeof(cfg.pass));
      if (j.containsKey("apSSID")) strlcpy(cfg.apSSID, j["apSSID"], sizeof(cfg.apSSID));
      if (j.containsKey("apPass")) strlcpy(cfg.apPass, j["apPass"], sizeof(cfg.apPass));
      saveConfig();
      req->send(200, "application/json", "{\"success\":true}");
      delay(1000); ESP.restart();
    } else { req->send(400); }
  }));

// --- WiFi Scan Async (non-blocking) ---
// Scan dijalankan di background, tidak freeze server
server.on("/api/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *req) {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_RUNNING) {
    // Masih scan, suruh client retry sebentar lagi
    req->send(202, "application/json", "{\"status\":\"scanning\"}");
    return;
  }
  if (n == WIFI_SCAN_FAILED || n == 0) {
    // Belum ada scan / gagal → mulai scan baru (async, tidak blocking)
    WiFi.scanNetworks(true); // true = async
    req->send(202, "application/json", "{\"status\":\"scanning\"}");
    return;
  }
  // Hasil scan sudah siap
  String j = "[";
  for (int i = 0; i < n; i++) {
    if (i) j += ",";
    j += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + ",\"secure\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false") + "}";
  }
  j += "]";
  WiFi.scanDelete(); // bebersih memory
  req->send(200, "application/json", j);
});

    server.on("/api/restart", HTTP_POST, [](AsyncWebServerRequest *req) {
      req->send(200, "application/json", "{\"success\":true}");
      delay(1000); ESP.restart();
    });

    server.on("/api/factory", HTTP_POST, [](AsyncWebServerRequest *req) {
      prefs.begin(CFG_NS, false); prefs.clear(); prefs.end();
      LittleFS.remove(DATA_FILE);
      req->send(200, "application/json", "{\"success\":true}");
      delay(1500); ESP.restart();
    });
  server.begin();
}

void setupWiFi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(cfg.apSSID, (strlen(cfg.apPass) >= 8) ? cfg.apPass : NULL);
  
  if (strlen(cfg.ssid) > 0) {
    WiFi.begin(cfg.ssid, cfg.pass);
    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 10000) {
      delay(500);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("=================================");
  Serial.println("STA IP: http://" + WiFi.localIP().toString());
  Serial.println("AP  IP: http://" + WiFi.softAPIP().toString());
  Serial.println("mDNS  : http://" + String(cfg.devName) + ".local");
  Serial.println("=================================");
  //SETUP NTP
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  }

}

void setup() {

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  setupUART_NANO();   
  loadConfig();
  LittleFS.begin(true);
  
  setupWiFi(); 

  MDNS.begin(cfg.devName);
  setupLED();
  setupRoutes();
  ArduinoOTA.setHostname(cfg.devName);
  ArduinoOTA.begin();
}

void loop() {
  //server.handleClient();
  ws2812fx.service();
  updateSearchStatus();
  ArduinoOTA.handle();
  readUART_ATMEGA(); 
  sendUART_NANO();
  ambilNTP();
  yield();
}