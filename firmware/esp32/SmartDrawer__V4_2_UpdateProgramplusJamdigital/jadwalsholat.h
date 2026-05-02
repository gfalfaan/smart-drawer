#ifndef JADWALSHOLAT_H
#define JADWALSHOLAT_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h> 

// ID Kota diset ke 1628 untuk wilayah Kota Semarang
const String KOTA_ID = "1628"; 

// ── 1. Fungsi Tembak API Sholat Kemenag ────────────────────────
void ambilJadwalSholat(int thn, int bln, int tgl) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    char url[80];
    sprintf(url, "https://api.myquran.com/v2/sholat/jadwal/%s/%04d/%02d/%02d", KOTA_ID.c_str(), thn, bln, tgl);
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      JsonDocument doc; // Support ArduinoJson v7
      deserializeJson(doc, payload);
      
      const char* subuh   = doc["data"]["jadwal"]["subuh"];
      const char* dzuhur  = doc["data"]["jadwal"]["dzuhur"];
      const char* ashar   = doc["data"]["jadwal"]["ashar"];
      const char* maghrib = doc["data"]["jadwal"]["maghrib"];
      const char* isya    = doc["data"]["jadwal"]["isya"];
      
      // Print ke PC
      Serial.print("JADWAL:");
      Serial.print(subuh);   Serial.print(",");
      Serial.print(dzuhur);  Serial.print(",");
      Serial.print(ashar);   Serial.print(",");
      Serial.print(maghrib); Serial.print(",");
      Serial.println(isya);

      // Print ke ATmega (Serial2)
      Serial2.print("JADWAL:");
      Serial2.print(subuh);   Serial2.print(",");
      Serial2.print(dzuhur);  Serial2.print(",");
      Serial2.print(ashar);   Serial2.print(",");
      Serial2.print(maghrib); Serial2.print(",");
      Serial2.println(isya);

      // 🔥 PERBAIKAN: Print ke Nano P10 (Serial1)
      Serial1.print("JADWAL:");
      Serial1.print(subuh);   Serial1.print(",");
      Serial1.print(dzuhur);  Serial1.print(",");
      Serial1.print(ashar);   Serial1.print(",");
      Serial1.print(maghrib); Serial1.print(",");
      Serial1.println(isya);
    }
    http.end();
  }
}

// ── 2. Fungsi Tembak API Hari Libur Nasional ──────────────────
void cekHariLibur(int thn, int bln, int tgl) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    char url[80];
    // Hit API Libur berdasarkan bulan dan tahun saat ini
    sprintf(url, "https://api-harilibur.vercel.app/api?month=%d&year=%04d", bln, thn);
    
    http.begin(url);
    int httpCode = http.GET();
    String hasilLibur = "KOSONG"; // Default jika tidak ada libur terdekat

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      JsonDocument doc; 
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        JsonArray arr = doc.as<JsonArray>();
        
        // Looping semua data libur di bulan ini
        for (JsonObject obj : arr) {
          bool isNational = obj["is_national_holiday"];
          
          if (isNational) {
            const char* dateStr = obj["holiday_date"]; // Formatnya "YYYY-MM-DD"
            const char* nameStr = obj["holiday_name"];
            
            // Ambil 2 digit angka terakhir (tanggalnya saja)
            int liburTgl = String(dateStr).substring(8, 10).toInt();

            if (liburTgl == tgl) {
              hasilLibur = String("HARI INI ") + nameStr;
              break; 
            } 
            else if (liburTgl > tgl && liburTgl <= tgl + 7) { 
              int selisih = liburTgl - tgl;
              if (selisih == 1) hasilLibur = String("BESOK ") + nameStr;
              else hasilLibur = String("H-") + String(selisih) + " " + nameStr;
              break; 
            }
          }
        }
      }
    }
    http.end();

    // Kirim hasil ke semua jalur
    Serial.print("LIBUR:");
    Serial.println(hasilLibur);
    
    Serial2.print("LIBUR:");
    Serial2.println(hasilLibur);

    // 🔥 PERBAIKAN: Print Libur ke Nano P10 (Serial1)
    Serial1.print("LIBUR:");
    Serial1.println(hasilLibur);
  }
}

// ── 3. Mesin Waktu Utama (Di-panggil di loop) ──────────────────
void ambilNTP() {
  static unsigned long lastJadwalUpdate = 0;
  static const unsigned long JADWAL_INTERVAL = 24UL * 60UL * 60UL * 1000UL; // 24 Jam
  static bool jadwalPertama = true;

  if (WiFi.status() == WL_CONNECTED) {
    unsigned long now = millis();
    
    if (jadwalPertama || (now - lastJadwalUpdate >= JADWAL_INTERVAL)) {
      struct tm timeinfo;
      
      // Tarik jam dari internet
      if (getLocalTime(&timeinfo)) {
        int thn = timeinfo.tm_year + 1900;
        int bln = timeinfo.tm_mon + 1;
        int tgl = timeinfo.tm_mday;
        
        // Eksekusi API
        ambilJadwalSholat(thn, bln, tgl);
        delay(500); 
        cekHariLibur(thn, bln, tgl); 
        
        lastJadwalUpdate = now;
        jadwalPertama = false;
      }
    }
  }
}

#endif