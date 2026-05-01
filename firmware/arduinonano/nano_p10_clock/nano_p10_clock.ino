// ============================================================
//  Smart Drawer — Panel P10 Clock Display  v3.0
// ============================================================

#include <SPI.h>
#include <DMD.h>
#include <MsTimer2.h>
#include "EMSans5x6.h"
#include "SystemFont5x7.h"
// #include "Arial_Black_16_ISO_8859_1.h"
#include "ElektronMart6x16.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN   1

#define SCROLL_SPEED_MS  50
#define BLINK_INTERVAL   500
#define DUR_MODE1_SEC    60
#define DUR_MODE2_SEC    30
#define FADE_STEPS       20
#define FADE_STEP_MS     30

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
void ScanDMD() { dmd.scanDisplayBySPI(); }

// ── UART ──────────────────────────────────────────────────
String timeFull = "--:--:--";
String tglStr   = "--/--/----";
String hariStr  = "----";
String uartBuf  = "";

const char* NAMA_HARI[] = {
  "MINGGU","SENIN","SELASA","RABU","KAMIS","JUMAT","SABTU"
};

int hitungHari(int d, int m, int y) {
  y += 2000;
  static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
  if (m < 3) y--;
  return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void parseData(String data) {
  data.trim();
  if (!data.length()) return;
  if (data.startsWith("TIME:")) {
    timeFull = data.substring(5);
  }
  else if (data.startsWith("DATE:")) {
    String raw = data.substring(5);
    if (raw.length() >= 8) {
      int dd = raw.substring(0,2).toInt();
      int mm = raw.substring(3,5).toInt();
      int yy = raw.substring(6,8).toInt();
      char buf[12];
      sprintf(buf, "%02d/%02d/20%02d", dd, mm, yy);
      tglStr  = String(buf);
      hariStr = String(NAMA_HARI[hitungHari(dd, mm, yy)]);
    }
  }
}

// ── State ─────────────────────────────────────────────────
byte          appState   = 0;
byte          nextState  = 2;
String        lastSec    = "";
int           secCounter = 0;
int           fadeStep   = 0;
unsigned long fadeLastT  = 0;

// ── Blink ─────────────────────────────────────────────────
bool          blinkOn    = true;
unsigned long blinkLastT = 0;

// ── Mode 1 ────────────────────────────────────────────────
void drawMode1() {
  if (timeFull.length() < 8) return;
  char hh[3], mm[3], ss[3];
  timeFull.substring(0,2).toCharArray(hh, 3);
  timeFull.substring(3,5).toCharArray(mm, 3);
  timeFull.substring(6,8).toCharArray(ss, 3);

  unsigned long now = millis();
  bool blinkChanged = false;

  if (now - blinkLastT >= BLINK_INTERVAL) {
    blinkLastT   = now;
    blinkOn      = !blinkOn;
    blinkChanged = true;
  }

  static String lastHH = "", lastMM = "", lastSS = "";
  bool timeChanged = (String(hh) != lastHH || String(mm) != lastMM || String(ss) != lastSS);
  if (!timeChanged && !blinkChanged) return;

  lastHH = String(hh);
  lastMM = String(mm);
  lastSS = String(ss);

  dmd.clearScreen(true);
  // HH font besar, maks x=14 supaya tidak nabrak titik dua
  dmd.selectFont(ElektronMart6x16);
  dmd.drawString(1, 0, hh, 2, GRAPHICS_NORMAL);

  // Titik dua tepat di tengah layar x=15,16
  if (blinkOn) {
    // titik atas (y=4,5)
    dmd.writePixel(16, 4, GRAPHICS_NORMAL, 1);
    dmd.writePixel(17, 4, GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 5, GRAPHICS_NORMAL, 1);
    dmd.writePixel(17, 5, GRAPHICS_NORMAL, 1);
    // titik bawah (y=9,10)
    dmd.writePixel(16, 10,  GRAPHICS_NORMAL, 1);
    dmd.writePixel(17, 10,  GRAPHICS_NORMAL, 1);
    dmd.writePixel(16, 11, GRAPHICS_NORMAL, 1);
    dmd.writePixel(17, 11, GRAPHICS_NORMAL, 1);
  }

  // MM kanan atas, SS kanan bawah mulai x=18
  dmd.selectFont(EMSans5x6);
  dmd.drawString(20, 0, mm, 2, GRAPHICS_NORMAL);
  dmd.drawString(20, 8, ss, 2, GRAPHICS_NORMAL);
}

// ── Mode 2 ────────────────────────────────────────────────
int           scX     = 32;
unsigned long scLastT = 0;
String        scText  = "";

void drawMode2() {
  String jam   = timeFull.substring(0, 5);
  String bawah = hariStr + " " + tglStr;
  if (bawah != scText) { scText = bawah; scX = 32; }

  unsigned long now = millis();
  bool doScroll = (now - scLastT >= SCROLL_SPEED_MS);
  if (doScroll) {
    scLastT = now;
    scX--;
    if (scX < -(int)(scText.length() * 6)) scX = 32;
  }

  static String lastJam = "";
  if (!doScroll && jam == lastJam) return;
  lastJam = jam;

dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  
  // Pisah Jam dan Menit (Tanpa titik dua bawaan teks)
  String hh = timeFull.substring(0, 2);
  String mm = timeFull.substring(3, 5);
  
  // 1. Tulis Jam (Mulai X = 3)
  dmd.drawString(3, 1, hh.c_str(), 2, GRAPHICS_NORMAL);
  
  // 2. Gambar Titik Dua Manual (Di tengah X = 15 & 16)
  // Atas
  dmd.writePixel(15, 3, GRAPHICS_NORMAL, 1);
  dmd.writePixel(16, 3, GRAPHICS_NORMAL, 1);
  // Bawah
  dmd.writePixel(15, 6, GRAPHICS_NORMAL, 1);
  dmd.writePixel(16, 6, GRAPHICS_NORMAL, 1);
  // 3. Tulis Menit (Mulai X = 18)
  dmd.drawString(18, 1, mm.c_str(), 2, GRAPHICS_NORMAL);
  
  // Tulis Teks Berjalan di baris bawah
  dmd.drawString(scX, 9, scText.c_str(), scText.length(), GRAPHICS_NORMAL);
}

// ── Fade ──────────────────────────────────────────────────
void doFade(unsigned long now) {
  unsigned long interval;
  if (fadeStep < FADE_STEPS / 2)
    interval = map(fadeStep, 0, FADE_STEPS/2, 60, 20);
  else
    interval = 10;

  if (now - fadeLastT >= interval) {
    fadeLastT = now;
    if (fadeStep % 2 == 0) {
      if (appState == 1) {
        if (timeFull.length() >= 8) {
          char hh[3];
          timeFull.substring(0,2).toCharArray(hh, 3);
          dmd.clearScreen(true);
          dmd.selectFont(ElektronMart6x16);
          dmd.drawString(0, 1, hh, 2, GRAPHICS_NORMAL);
        }
      } else {
        String jam = timeFull.substring(0, 5);
        dmd.clearScreen(true);
        dmd.selectFont(SystemFont5x7);
        int xJam = max(0, (32 - (int)jam.length() * 6) / 2);
        dmd.drawString(xJam, 1, jam.c_str(), jam.length(), GRAPHICS_NORMAL);
      }
    } else {
      dmd.clearScreen(true);
    }
    fadeStep++;
  }

  if (fadeStep >= FADE_STEPS) {
    dmd.clearScreen(true);
    delay(150);
    appState   = nextState;
    secCounter = 0;
    fadeStep   = 0;
    if (appState == 2) scX = 32;
  }
}

// ── Setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  MsTimer2::set(2, ScanDMD);
  MsTimer2::start();

  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(2, 4, "TUNGGU YAA", 4, GRAPHICS_NORMAL);
  delay(1500);
  dmd.clearScreen(true);

  appState   = 0;
  nextState  = 2;
  secCounter = 0;
  lastSec    = "";
  fadeStep   = 0;
  blinkOn    = true;
  blinkLastT = millis();
  scLastT    = millis();
  scX        = 32;
}

// ── Loop ──────────────────────────────────────────────────
void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { parseData(uartBuf); uartBuf = ""; }
    else if (c != '\r') uartBuf += c;
  }

  if (timeFull.length() >= 8) {
    String curSec = timeFull.substring(6, 8);
    if (curSec != lastSec && (appState == 0 || appState == 2)) {
      lastSec = curSec;
      secCounter++;
    }
  }

  unsigned long now = millis();

  switch (appState) {
    case 0:
      drawMode1();
      if (secCounter >= DUR_MODE1_SEC) {
        appState  = 1;
        nextState = 2;
        fadeStep  = 0;
        fadeLastT = now;
      }
      break;
    case 1:
      doFade(now);
      break;
    case 2:
      drawMode2();
      if (secCounter >= DUR_MODE2_SEC) {
        appState  = 3;
        nextState = 0;
        fadeStep  = 0;
        fadeLastT = now;
      }
      break;
    case 3:
      doFade(now);
      break;
  }
}