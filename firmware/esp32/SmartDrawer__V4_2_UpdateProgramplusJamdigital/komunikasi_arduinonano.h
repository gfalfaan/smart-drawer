#ifndef KOMUNIKASI_ARDUINONANO_H
#define KOMUNIKASI_ARDUINONANO_H
// ═══════════════════════════════════════════════════════════════
//  komunikasi_arduinonano.h
//  Kirim data jam & tanggal dari ESP32 ke Arduino Nano (Panel P10)
//
//  Wiring:
//    ESP32 GPIO4 (TX Serial1) ──→ Arduino Nano D0 (RX)
//    GND ESP32                ──→ GND Nano  (WAJIB!)
//
//  Serial yang dipakai:
//    Serial  = USB debug  (115200)
//    Serial2 = ATmega 16L (RX=16, TX=17, 9600)  ← sudah ada
//    Serial1 = Nano P10   (TX=4,  RX=-1, 9600)  ← file ini
//
//  Data yang dikirim ke Nano (tiap 1 detik):
//    TIME:HH:MM:SS\n
//    DATE:DD/MM/YY\n
// ═══════════════════════════════════════════════════════════════
// ── Init Serial1 ke Nano ──────────────────────────────────────
// Panggil sekali di setup() setelah Serial2 diinit
void setupUART_NANO() {
  // TX=4, RX tidak dipakai (-1), baud 9600 sama dengan Nano
  Serial1.begin(9600, SERIAL_8N1, -1, 4);
  Serial.println("[Nano] Serial1 TX=GPIO4 ready");
}

// ── Kirim jam & tanggal ke Nano ───────────────────────────────
// Dipanggil dari loop() — otomatis throttle 1x per detik
// Bergantung pada rtcTime & rtcDate dari komunikasi_atmega.h
void sendUART_NANO() {
  static unsigned long lastSend = 0;
  static String lastTime = "";
  static String lastDate = "";

  // Kirim hanya tiap 1 detik
  if (millis() - lastSend < 1000) return;
  lastSend = millis();

  // Jangan kirim kalau data belum masuk dari ATmega
  if (rtcTime.startsWith("--") || rtcDate.startsWith("--")) return;

  // Kirim TIME hanya kalau ada perubahan (hemat bandwidth)
  if (rtcTime != lastTime) {
    Serial1.println("TIME:" + rtcTime);
    Serial.println("[->Nano] TIME:" + rtcTime); // debug
    lastTime = rtcTime;
  }

  // Kirim DATE hanya kalau ada perubahan
  if (rtcDate != lastDate) {
    Serial1.println("DATE:" + rtcDate);
    Serial.println("[->Nano] DATE:" + rtcDate); // debug
    lastDate = rtcDate;
  }
}

#endif