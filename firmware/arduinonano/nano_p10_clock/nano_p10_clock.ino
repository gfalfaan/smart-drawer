// ============================================================
//  Smart Drawer — Panel P10 Clock Display  v11.0 (Snow Burst)
// ============================================================

#include <SPI.h>
#include <DMD.h>
#include <MsTimer2.h>
#include "EMSans5x6.h"
#include "SystemFont5x7.h"
#include "ElektronMart6x16.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN   1

#define SCROLL_SPEED_MS  50
#define BLINK_INTERVAL   500

// ── KONFIGURASI DURASI ──────────────────────────────────────
#define DUR_MODE1_SEC    80 
#define DUR_MODE2_SEC    25  
#define DUR_SLIDE_SEC    3   
#define DUR_LIBUR_MAX    20  

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
void ScanDMD() { dmd.scanDisplayBySPI(); }

// ── UART & Variabel Data ──────────────────────────────────
String timeFull = "--:--:--";
String tglStr   = "--/--/----";
String hariStr  = "----";
String uartBuf  = "";

const char* nama_sholat[] = {"SUBUH", "DZUHR", "ASHAR", "MAGRB", "ISYA"};
unsigned char jadwal_hari_ini[5][2] = { {4,30},{11,45},{15,00},{17,45},{19,00} };
String namaSholat = "";
String teksLibur = ""; 

const char* NAMA_HARI[] = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUMAT","SABTU"};

byte appState = 0;
int  secCounter = 0;
String lastSec = "";
bool blinkOn = true;
unsigned long blinkLastT = 0;

// Tracker Anti-Flicker
String lHH1="", lMM1="", lSS1="", lJam2="";

int hitungHari(int d, int m, int y) {
  y += 2000;
  static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
  if (m < 3) y--;
  return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void parseData(String data) {
  data.trim();
  if (!data.length()) return;
  if (data.startsWith("TIME:")) timeFull = data.substring(5);
  else if (data.startsWith("DATE:")) {
    String raw = data.substring(5);
    if (raw.length() >= 8) {
      int dd = raw.substring(0,2).toInt();
      int mm = raw.substring(3,5).toInt();
      int yy = raw.substring(6,8).toInt();
      char buf[12];
      sprintf(buf, "%02d/%02d/20%02d", dd, mm, yy);
      tglStr = String(buf);
      hariStr = String(NAMA_HARI[hitungHari(dd, mm, yy)]);
    }
  }
  else if (data.startsWith("JADWAL:")) {
    String raw = data.substring(7);
    if (raw.length() >= 29) {
      for(int i=0; i<5; i++) {
        jadwal_hari_ini[i][0] = raw.substring(i*6, (i*6)+2).toInt();
        jadwal_hari_ini[i][1] = raw.substring((i*6)+3, (i*6)+5).toInt();
      }
    }
  }
  else if (data.startsWith("LIBUR:")) {
    String raw = data.substring(6);
    teksLibur = (raw == "" || raw == "KOSONG") ? "" : raw + "     ";
  }
}

// ── EFEK SNOW BURST ASLI KARYA GHOFUR  ──────────────────
void efekSnowBurst() {
  for (int fadeStep = 0; fadeStep <= 25; fadeStep++) {
    for (int x = 0; x < 32; x++) {
      for (int y = 0; y < 16; y++) {
        int dist = abs(x - 15) + abs(y - 7) * 2;
        if (dist < fadeStep * 2) {
          if (random(100) < 85) dmd.writePixel(x, y, GRAPHICS_NORMAL, 0);
        } else if (dist < fadeStep * 2 + 5) {
          int randVal = random(100);
          if (randVal < 15) dmd.writePixel(x, y, GRAPHICS_NORMAL, 1);
          else if (randVal < 40) dmd.writePixel(x, y, GRAPHICS_NORMAL, 0);
        }
      }
    }
    delay(35); // Kecepatan ledakan efek (35ms)
  }
  dmd.clearScreen(true);
  delay(150); // Jeda bentar biar smooth sebelum mode baru
}

// ── Mode functions ─────────────────────────────────────────
void drawMode1() {
  if (timeFull.length() < 8) return;
  String hh = timeFull.substring(0,2);
  String mm = timeFull.substring(3,5);
  String ss = timeFull.substring(6,8);
  unsigned long now = millis();
  
  if (now - blinkLastT >= BLINK_INTERVAL) {
    blinkLastT = now; blinkOn = !blinkOn;
    byte color = blinkOn ? 1 : 0;
    dmd.writePixel(16,4,GRAPHICS_NORMAL,color); dmd.writePixel(17,4,GRAPHICS_NORMAL,color);
    dmd.writePixel(16,5,GRAPHICS_NORMAL,color); dmd.writePixel(17,5,GRAPHICS_NORMAL,color);
    dmd.writePixel(16,10,GRAPHICS_NORMAL,color); dmd.writePixel(17,10,GRAPHICS_NORMAL,color);
    dmd.writePixel(16,11,GRAPHICS_NORMAL,color); dmd.writePixel(17,11,GRAPHICS_NORMAL,color);
  }
  
  if (hh != lHH1) { dmd.selectFont(ElektronMart6x16); dmd.drawString(1,0,lHH1.c_str(),2,GRAPHICS_NOR); dmd.drawString(1,0,hh.c_str(),2,GRAPHICS_NORMAL); lHH1=hh; }
  if (mm != lMM1) { dmd.selectFont(EMSans5x6); dmd.drawString(20,0,lMM1.c_str(),2,GRAPHICS_NOR); dmd.drawString(20,0,mm.c_str(),2,GRAPHICS_NORMAL); lMM1=mm; }
  if (ss != lSS1) { dmd.selectFont(EMSans5x6); dmd.drawString(20,8,lSS1.c_str(),2,GRAPHICS_NOR); dmd.drawString(20,8,ss.c_str(),2,GRAPHICS_NORMAL); lSS1=ss; }
}

void drawMode2() {
  String jam = timeFull.substring(0,5);
  String tks = hariStr + " " + tglStr;
  
  if (jam != lJam2) {
    dmd.drawFilledBox(0, 0, 31, 8, GRAPHICS_NOR); 
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(3, 1, jam.substring(0,2).c_str(), 2, GRAPHICS_NORMAL);
    dmd.drawString(18, 1, jam.substring(3,5).c_str(), 2, GRAPHICS_NORMAL);  
    dmd.writePixel(15, 3, GRAPHICS_NORMAL, 1);
    dmd.writePixel(15, 2, GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 3, GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 2, GRAPHICS_NORMAL, 1);
    dmd.writePixel(15, 5, GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 5, GRAPHICS_NORMAL, 1);
    dmd.writePixel(15, 6, GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 6, GRAPHICS_NORMAL, 1);
    lJam2 = jam;
  }
  
  static unsigned long scLastT = 0; static int scX = 32;
  if (millis() - scLastT >= 50) {
    scLastT = millis(); dmd.selectFont(SystemFont5x7);
    dmd.drawString(scX, 9, tks.c_str(), tks.length(), GRAPHICS_NOR);
    if (--scX < -(int)(tks.length()*6)) scX = 32;
    dmd.drawString(scX, 9, tks.c_str(), tks.length(), GRAPHICS_NORMAL);
  }
}

void drawModeJadwal() {
  int idx = (secCounter / DUR_SLIDE_SEC); if (idx > 4) idx = 4;
  static int lastIdx = -1;
  if (idx != lastIdx) {
    dmd.clearScreen(true); dmd.selectFont(SystemFont5x7);
    dmd.drawString(1,1,nama_sholat[idx], strlen(nama_sholat[idx]), GRAPHICS_NORMAL);
    char jb[6]; sprintf(jb, "%02d:%02d", jadwal_hari_ini[idx][0], jadwal_hari_ini[idx][1]);
    dmd.drawString(1,9, (jadwal_hari_ini[idx][0]==0 && jadwal_hari_ini[idx][1]==0) ? "--:--" : jb, 5, GRAPHICS_NORMAL);
    lastIdx = idx;
  }
  if (secCounter >= (DUR_SLIDE_SEC * 5)) lastIdx = -1; 
}

void drawModeLibur() {
  static int lx = 32; static unsigned long lt = 0;
  if (millis() - lt >= 50) {
    lt = millis(); dmd.selectFont(SystemFont5x7);
    dmd.drawString(lx,5,teksLibur.c_str(),teksLibur.length(),GRAPHICS_NOR);
    if (--lx < -(int)(teksLibur.length()*6)) lx=32;
    dmd.drawString(lx,5,teksLibur.c_str(),teksLibur.length(),GRAPHICS_NORMAL);
  }
}

void drawModeAdzan() {
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(1,1,"ADZAN",5,GRAPHICS_NORMAL);
  dmd.drawString(1,9,namaSholat.c_str(),namaSholat.length(),GRAPHICS_NORMAL);
}

void setup() {
  Serial.begin(9600); MsTimer2::set(2, ScanDMD); MsTimer2::start();
  dmd.clearScreen(true);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { parseData(uartBuf); uartBuf = ""; }
    else if (c != '\r') uartBuf += c;
  }

  if (timeFull.length() >= 8) {
    String curSec = timeFull.substring(6, 8);
    if (curSec != lastSec) {
      lastSec = curSec; secCounter++;
      int curHH = timeFull.substring(0, 2).toInt();
      int curMM = timeFull.substring(3, 5).toInt();
      
      // PRIORITAS ADZAN
      if (curSec == "00") {
        for (int i=0; i<5; i++) {
          if (curHH == jadwal_hari_ini[i][0] && curMM == jadwal_hari_ini[i][1]) {
            namaSholat = String(nama_sholat[i]); appState = 4; secCounter = 0; 
            dmd.clearScreen(true); 
            lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
          }
        }
      }
    }
  }

  switch (appState) {
    case 0: drawMode1(); 
            if (secCounter >= DUR_MODE1_SEC) { 
              efekSnowBurst(); appState=2; secCounter=0; 
              lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
            } break;
    case 2: drawMode2(); 
            if (secCounter >= DUR_MODE2_SEC) { 
              efekSnowBurst(); appState=5; secCounter=0; 
              lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
            } break;
    case 5: drawModeJadwal(); 
            if (secCounter >= (DUR_SLIDE_SEC * 5)) { 
              efekSnowBurst(); 
              appState = (teksLibur != "") ? 7 : 0; secCounter = 0; 
              lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
            } break;
    case 7: drawModeLibur(); 
            if (secCounter >= DUR_LIBUR_MAX) { 
              efekSnowBurst(); appState=0; secCounter=0; 
              lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
            } break;
    case 4: drawModeAdzan(); 
            if (secCounter >= 60) { 
              efekSnowBurst(); appState=0; secCounter=0; 
              lHH1=""; lMM1=""; lSS1=""; lJam2=""; // RESET LANGSUNG
            } break;
  }
}