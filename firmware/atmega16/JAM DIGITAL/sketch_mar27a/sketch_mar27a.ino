#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define BTN_SET   0   // PINB.0
#define BTN_UP    1   // PINB.1
#define BTN_DOWN  2   // PINB.2
#define BUZZER    14  // PORTD.6
#define LED_PIN   4   // PORTB.4
#define NUM_LEDS  9

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Variabel Global
unsigned char jam, menit, detik, hari, tgl, bln;
int thn;
unsigned char detik_lama = 255;
unsigned char menu_pos = 0;
unsigned char menu_aktif = 0;
unsigned char mode_lampu;
signed char edit_val = 0; 
unsigned char display_timer = 0;

unsigned char alm_h, alm_m, alm_on, nada_alarm;

bool popup_event = false;
unsigned char last_event_min = 255;
char buf[17]; // buffer universal LCD (aman)

const char nama_hari[][10] PROGMEM = {
  "MINGGU","SENIN","SELASA","RABU","KAMIS","JUMAT","SABTU"
};
const char nama_nada[][10] PROGMEM = {
  "SIREN","RAPID","EMERG","KONTI","TRIPLE"
};
const char nama_menu[][16] PROGMEM = {
  "JAM","MENIT","TANGGAL","BULAN","TAHUN",
  "ALARM JAM","ALARM MENIT","ALM ON/OFF",
  "NADA ALARM","SETUP LAMPU","KELUAR"
};
const char nama_warna[][16] PROGMEM = {
  "MATI",
  "PUTIH",
  "KUNING",
  "BIRU",
  "PINK",
  "PELANGI",
  "NAPAS RGB",
  "ICE BLUE",
  "CYAN",
  "HIJAU",
  "UNGU",
  "WARM ROOM"
};
const char nama_sholat[][10] PROGMEM  = {"SUBUH", "DZUHUR", "ASHAR", "MAGHRIB", "ISYA"}; // SUDAH DITAMBAHKAN

unsigned char jadwal_semarang[12][5][2] = {
    {{4,20},{11,47},{15,12},{18,02},{19,17}}, {{4,28},{11,53},{15,03},{18,05},{19,16}}, 
    {{4,28},{11,49},{14,56},{17,56},{19,05}}, {{4,24},{11,41},{14,57},{17,42},{18,51}}, 
    {{4,21},{11,37},{14,58},{17,35},{18,47}}, {{4,25},{11,40},{15,01},{17,36},{18,50}}, 
    {{4,30},{11,45},{15,06},{17,41},{18,54}}, {{4,26},{11,43},{15,01},{17,41},{18,52}}, 
    {{4,12},{11,32},{14,44},{17,33},{18,42}}, {{3,56},{11,21},{14,24},{17,25},{18,36}}, 
    {{3,47},{11,20},{14,34},{17,29},{18,42}}, {{3,54},{11,32},{14,53},{17,43},{18,57}}
};

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  if (WheelPos < 170) { WheelPos -= 85; return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3); }
  WheelPos -= 170; return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setup() {
  
  Serial.begin(9600);
  Serial.setTimeout(10);

  pinMode(BTN_SET, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Wire.begin(); rtc.begin(); lcd.init(); lcd.backlight(); strip.begin();strip.setBrightness(255);

  mode_lampu = EEPROM.read(10); alm_h = EEPROM.read(11); 
  alm_m = EEPROM.read(12); alm_on = EEPROM.read(13); nada_alarm = EEPROM.read(14);
  
  if (alm_h > 23) alm_h=6; if (alm_m > 59) alm_m=0; if (alm_on > 1) alm_on=1;
  if (nada_alarm < 1 || nada_alarm > 5) nada_alarm=1;
  if (mode_lampu > 11) mode_lampu=0;
  loading_animation();
}

void loop() {
  // ===== AMBIL WAKTU DULU (WAJIB DI ATAS) =====
  DateTime now = rtc.now();
  jam = now.hour();
  menit = now.minute();
  detik = now.second();
  tgl = now.day();
  bln = now.month();
  thn = now.year() - 2000;
  hari = hitung_hari(tgl, bln, thn);


  // ===== TERIMA DATA DARI ESP32 =====
  if (Serial.available()) {
  char cmd[40];
    int len = Serial.readBytesUntil('\n', cmd, sizeof(cmd) - 1);
    cmd[len] = '\0';  // 🔥 WAJIB: nutup string
    cmd[strcspn(cmd, "\r")] = 0;  // 🔥 HAPUS \r
    // ===== SET TIME =====
    if (strncmp(cmd, "SETTIME:", 8) == 0) {
      int h,m,s;
      sscanf(cmd, "SETTIME:%d:%d:%d", &h,&m,&s);
      rtc.adjust(DateTime(2000+thn, bln, tgl, h, m, s));
    }

    // ===== SET ALARM =====
    else if (strncmp(cmd, "SETALARM:", 9) == 0) {
      int h, m;
      if (sscanf(cmd, "SETALARM:%d:%d", &h, &m) == 2) {
        alm_h = h;
        alm_m = m;
      }
      EEPROM.write(11, alm_h);
      EEPROM.write(12, alm_m);

      lcd.clear();        // 🔥 paksa refresh
      update_display();   // 🔥 tampilkan ulang
    }

    // ===== SET MODE =====
    else if (strncmp(cmd, "SETMODE:", 8) == 0) {
      int m;
      sscanf(cmd, "SETMODE:%d", &m);
      if (m >= 0 && m <= 11) {
        mode_lampu = m;
        EEPROM.write(10, mode_lampu);
      }
    }

    // ===== SET TONE =====
    else if (strncmp(cmd, "SETTONE:", 8) == 0) {
      int t;
      sscanf(cmd, "SETTONE:%d", &t);
      if (t >= 1 && t <= 5) {
        nada_alarm = t;
        EEPROM.write(14, nada_alarm);
      }
    }

    // ===== SET ALARM ON/OFF =====
    else if (strncmp(cmd, "SETALARMON:", 11) == 0) {
      int on;
      sscanf(cmd, "SETALARMON:%d", &on);
      if (on == 0 || on == 1) {
        alm_on = on;
        EEPROM.write(13, alm_on);

        lcd.clear();        // 🔥 paksa refresh
        update_display();   // 🔥 tampilkan ulang
      }
    }
  }
  // ===== KIRIM DATA KE ESP32 =====
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {
    lastSend = millis();

    Serial.print(F("TIME:"));
    if (jam < 10) Serial.print('0');
    Serial.print(jam);
    Serial.print(':');
    if (menit < 10) Serial.print('0');
    Serial.print(menit);
    Serial.print(':');
    if (detik < 10) Serial.print('0');
    Serial.println(detik);

    Serial.print(F("ALARM:"));
    if (alm_h < 10) Serial.print('0');
    Serial.print(alm_h);
    Serial.print(':');
    if (alm_m < 10) Serial.print('0');
    Serial.println(alm_m);

      // 🔥 TAMBAHAN DATE (INI YANG KAMU BUTUH)
    Serial.print(F("DATE:"));
    if (tgl < 10) Serial.print('0');
    Serial.print(tgl);
    Serial.print('/');
    if (bln < 10) Serial.print('0');
    Serial.print(bln);
    Serial.print('/');
    if (thn < 10) Serial.print('0');
    Serial.println(thn);

    Serial.print(F("ALARMON:"));
    Serial.println(alm_on);
  }
  if (menu_pos == 0 && menu_aktif == 0) {
    if (detik != detik_lama) { detik_lama = detik; update_display(); }
  }

  static int last_alm = -1;

  if (last_alm != alm_on) {
    last_alm = alm_on;
    lcd.clear();
    update_display();
  }

  refresh_lampu();
  cek_alarm_dan_sholat();

  if (digitalRead(BTN_SET) == LOW) {
    delay(30);
    if (digitalRead(BTN_SET) == LOW) {
      beep();
      if (menu_pos == 0 && menu_aktif == 0) { menu_pos = 1; lcd.clear(); }
      else if (menu_aktif == 0 && menu_pos >= 1 && menu_pos <= 10) {
        menu_aktif = 1;
        if(menu_pos==1) edit_val = jam; else if(menu_pos==2) edit_val = menit;
        else if(menu_pos==3) edit_val = tgl; else if(menu_pos==4) edit_val = bln;
        else if(menu_pos==5) edit_val = thn; else if(menu_pos==6) edit_val = alm_h;
        else if(menu_pos==7) edit_val = alm_m; else if(menu_pos==8) edit_val = alm_on;
        else if(menu_pos==9) edit_val = nada_alarm; else if(menu_pos==10) edit_val = mode_lampu;
        lcd.clear();
      }
      else if (menu_aktif == 1) {
        simpan_data(); menu_aktif = 0; lcd.clear();
      }
      else if (menu_pos == 11) { menu_pos = 0; menu_aktif = 0; lcd.clear(); }
      update_display();
      while(digitalRead(BTN_SET) == LOW);
    }
  }
//tombol up
  if (digitalRead(BTN_UP) == LOW) {
    delay(30); if(digitalRead(BTN_UP) == LOW){
      beep();
      if(menu_aktif == 0){
        menu_pos++; if (menu_pos > 11) menu_pos = 0; lcd.clear();
      } else {
        edit_val++; 
        if (menu_pos==1 && edit_val>23) edit_val=0; if (menu_pos==2 && edit_val>59) edit_val=0;
        if (menu_pos==3 && edit_val>31) edit_val=1; if (menu_pos==4 && edit_val>12) edit_val=1;
        if (menu_pos==5 && edit_val>99) edit_val=0; if (menu_pos==6 && edit_val>23) edit_val=0;
        if (menu_pos==7 && edit_val>59) edit_val=0; if (menu_pos==8 && edit_val>1) edit_val=0;
        if (menu_pos==9 && edit_val>5) edit_val=1; if (menu_pos==10 && edit_val>11) edit_val=0;
      }
      update_display(); while(digitalRead(BTN_UP) == LOW);
    }
  }
//tombol down
  if (digitalRead(BTN_DOWN) == LOW) {
    delay(30); if(digitalRead(BTN_DOWN) == LOW){
      beep();
      if(menu_aktif == 0){
        if (menu_pos == 0) menu_pos = 11; else menu_pos--; lcd.clear();
      } else {
        edit_val--;
        if (menu_pos==1 && edit_val<0) edit_val=23; if (menu_pos==2 && edit_val<0) edit_val=59;
        if (menu_pos==3 && edit_val<1) edit_val=31; if (menu_pos==4 && edit_val<1) edit_val=12;
        if (menu_pos==5 && edit_val<0) edit_val=99; if (menu_pos==6 && edit_val<0) edit_val=23;
        if (menu_pos==7 && edit_val<0) edit_val=59; if (menu_pos==8 && edit_val<0) edit_val=1;
        if (menu_pos==9 && edit_val<1) edit_val=5; if (menu_pos==10 && edit_val<0) edit_val=11;
      }
      update_display(); while(digitalRead(BTN_DOWN) == LOW);
    }
  }
}

void cek_alarm_dan_sholat() {
  if (menu_pos != 0 || menu_aktif != 0) return;

  // alarm biasa
  if (alm_on && jam == alm_h && menit == alm_m && detik == 0 && last_event_min != menit) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("ALARM AKTIF"));
    lcd.setCursor(3, 1);
    lcd.print(F("TANGI SUU!"));

    bunyi_alarm_event();

    last_event_min = menit;
    delay(1500);
    lcd.clear();
    update_display();
    return;
  }

  // alarm sholat
  for (int i = 0; i < 5; i++) {
    if (jam == jadwal_semarang[bln - 1][i][0] &&
        menit == jadwal_semarang[bln - 1][i][1] &&
        detik == 0 &&
        last_event_min != menit) {

      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(F("WAKTU SHOLAT"));
      lcd.setCursor(3, 1);
      strcpy_P(buf, nama_sholat[i]);
      lcd.print(buf);

      bunyi_alarm_solat();

      last_event_min = menit;
      delay(1500);
      lcd.clear();
      update_display();
      return;
    }
  }
}

void update_display() {
  if (menu_pos == 0 && menu_aktif == 0) {
    display_timer++; if (display_timer > 131) { display_timer = 0; lcd.clear(); }
    if (display_timer <= 120) {
      if (display_timer == 1) lcd.clear();
      lcd.setCursor(0, 0);
      strcpy_P(buf, nama_hari[hari]);
      lcd.print(buf);
      // hitung panjang kata
      int len = strlen(buf);
      // total mau 8 karakter (biar rapi)
      for(int i = len; i < 8; i++) {
        lcd.print(' ');
      }
      tampil_angka_lcd(tgl);
      lcd.print(F("/"));
      tampil_angka_lcd(bln);
      lcd.print(F("/"));
      tampil_angka_lcd(thn);
      lcd.setCursor(0, 1);
      tampil_angka_lcd(jam); lcd.print(detik % 2 == 0 ? F(":") : F(" "));
      tampil_angka_lcd(menit); lcd.print(detik % 2 == 0 ? F(":") : F(" "));
      tampil_angka_lcd(detik);
      if (alm_on) { lcd.print(F(" A")); tampil_angka_lcd(alm_h); lcd.print(F(":")); tampil_angka_lcd(alm_m); }
    } else {
      int s_idx = (display_timer - 122) / 2;
      if (s_idx >= 0 && s_idx < 5 && display_timer % 2 == 0) {
        lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("JADWAL SHOLAT"));
        lcd.setCursor(0, 1);strcpy_P(buf,nama_sholat[s_idx]);
        lcd.print(buf);lcd.print(F(" "));
        tampil_angka_lcd(jadwal_semarang[bln - 1][s_idx][0]); lcd.print(F(":")); tampil_angka_lcd(jadwal_semarang[bln - 1][s_idx][1]);
      }
    }
  } 
  else if (menu_aktif == 0) {
    lcd.setCursor(0, 0); lcd.print(F("MENU:           "));
    lcd.setCursor(0, 1); lcd.print(F(">")); strcpy_P(buf, nama_menu[menu_pos - 1]);
    lcd.print(buf); lcd.print(F("            "));
  } 
  else {
    lcd.setCursor(0, 0); lcd.print(F("EDIT:")); lcd.print(nama_menu[menu_pos - 1]);
    lcd.setCursor(0, 1);
    if (menu_pos == 10) { lcd.print(F("MODE: ")); strcpy_P(buf, nama_warna[edit_val]);
    lcd.print(buf); }
    else if (menu_pos == 8) { if(edit_val) lcd.print(F("[ON]  ")); else lcd.print(F("[OFF] ")); }
    else if (menu_pos == 9) { lcd.print(F("[")); lcd.print((int)edit_val); lcd.print(F("] ")); strcpy_P(buf, nama_nada[edit_val]);
    lcd.print(buf); }
    else if (menu_pos == 5) { lcd.print(F("THN: 20")); tampil_angka_lcd(edit_val); }
    else if (menu_pos == 1 || menu_pos == 6) { lcd.print(F("JAM: ")); tampil_angka_lcd(edit_val); }
    else if (menu_pos == 2 || menu_pos == 7) { lcd.print(F("MENIT: ")); tampil_angka_lcd(edit_val); }
    else { lcd.print(F("VAL: ")); tampil_angka_lcd(edit_val); }
    lcd.print(F("      "));
  }
}

void simpan_data() {
  if (menu_pos == 1) rtc.adjust(DateTime(2000+thn, bln, tgl, edit_val, menit, 0));
  else if (menu_pos == 2) rtc.adjust(DateTime(2000+thn, bln, tgl, jam, edit_val, 0));
  else if (menu_pos == 3) rtc.adjust(DateTime(2000+thn, bln, edit_val, jam, menit, 0));
  else if (menu_pos == 4) rtc.adjust(DateTime(2000+thn, edit_val, tgl, jam, menit, 0));
  else if (menu_pos == 5) rtc.adjust(DateTime(2000+edit_val, bln, tgl, jam, menit, 0));
  else if (menu_pos == 6) alm_h = edit_val;
  else if (menu_pos == 7) alm_m = edit_val;
  else if (menu_pos == 8) alm_on = edit_val;
  else if (menu_pos == 9) nada_alarm = edit_val;
  else if (menu_pos == 10) mode_lampu = edit_val;

  EEPROM.write(10, mode_lampu); EEPROM.write(11, alm_h);
  EEPROM.write(12, alm_m); EEPROM.write(13, alm_on);
  EEPROM.write(14, nada_alarm);
  lcd.clear(); lcd.setCursor(4, 0); lcd.print(F("SAVED!")); delay(800);
}

void loading_animation() {
  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("SMARTCLOCK......"));
  lcd.setCursor(0, 1); for (int i = 0; i < 16; i++) { lcd.write(255); delay(200); }
  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("GFALFAAN"));
  lcd.setCursor(4, 1); lcd.print(F("JAM DIGITAL"));
  digitalWrite(BUZZER, HIGH); delay(300); digitalWrite(BUZZER, LOW);
  delay(1500); lcd.clear();
}

void refresh_lampu() {
  static uint16_t j = 0;
  static int b_val = 50;
  static int b_dir = 1;

  if (mode_lampu == 1) {
    strip.fill(strip.Color(150, 150, 100));
  }
  else if (mode_lampu == 2) {
    strip.fill(strip.Color(255, 120, 0));
  }
  else if (mode_lampu == 3) {
    strip.fill(strip.Color(0, 0, 255));
  }
  else if (mode_lampu == 4) {
    strip.fill(strip.Color(255, 0, 150));
  }
  else if (mode_lampu == 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    j++;
  }
  else if (mode_lampu == 6) {
    strip.fill(strip.Color(0, b_val, b_val / 2));
    b_val += b_dir;
    if (b_val >= 200 || b_val <= 10) b_dir *= -1;
  }
  else if (mode_lampu == 7) {
    strip.fill(strip.Color(0, 200, 255)); // ICE BLUE
  }
  else if (mode_lampu == 8) {
    strip.fill(strip.Color(120, 255, 255)); // CYAN
  }
  else if (mode_lampu == 9) {
    strip.fill(strip.Color(0, 255, 80)); // HIJAU
  }
  else if (mode_lampu == 10) {
    strip.fill(strip.Color(180, 80, 255)); // UNGU
  }
  else if (mode_lampu == 11) {
    strip.fill(strip.Color(255, 220, 160)); // WARM ROOM
  }
  else {
    strip.fill(0);
  }

  strip.show();
}

unsigned char hitung_hari(unsigned char d, unsigned char m, unsigned char y) {
  int th = 2000 + y;
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) th -= 1;
  return (th + th/4 - th/100 + th/400 + t[m-1] + d) % 7 ;
}

void tampil_angka_lcd(unsigned char n) { 
  if (n < 10) lcd.print(F("0")); 
  lcd.print(n); 
}

void beep() { 
  digitalWrite(BUZZER, HIGH); 
  delay(30); 
  digitalWrite(BUZZER, LOW); 
}

void bunyi_alarm_solat() {
  int repeat = 6 + nada_alarm * 2;

  for (int i = 0; i < repeat; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(80 + (nada_alarm * 20));
    digitalWrite(BUZZER, LOW);
    delay(60);
  }
}

void bunyi_alarm_event() {
  // bunyi terus sampai salah satu tombol ditekan
  while (digitalRead(BTN_SET) == HIGH &&
         digitalRead(BTN_UP) == HIGH &&
         digitalRead(BTN_DOWN) == HIGH) {

    if (nada_alarm == 1) {
      digitalWrite(BUZZER, HIGH); delay(120);
      digitalWrite(BUZZER, LOW);  delay(120);
    }
    else if (nada_alarm == 2) {
      digitalWrite(BUZZER, HIGH); delay(60);
      digitalWrite(BUZZER, LOW);  delay(60);
    }
    else if (nada_alarm == 3) {
      digitalWrite(BUZZER, HIGH); delay(300);
      digitalWrite(BUZZER, LOW);  delay(120);
    }
    else if (nada_alarm == 4) {
      digitalWrite(BUZZER, HIGH); delay(500);
      digitalWrite(BUZZER, LOW);  delay(80);
    }
    else {
      for (int k = 0; k < 3; k++) {
        digitalWrite(BUZZER, HIGH); delay(100);
        digitalWrite(BUZZER, LOW);  delay(80);
      }
      delay(200);
    }
  }

  // pastikan buzzer mati setelah tombol ditekan
  digitalWrite(BUZZER, LOW);
}