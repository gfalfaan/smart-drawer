// #ifndef KOMUNIKASI_ATMEGA_H
// #define KOMUNIKASI_ATMEGA_H

// String rtcTime = "--:--:--";
// String rtcAlarm = "--:--";
// int rtcAlarmOn = 0;

// void readUART_ATMEGA() {
//   while (Serial2.available()) {
//   String data = Serial2.readStringUntil('\n');
//   Serial.print("DAPAT: ");
//   Serial.println(data);   // 🔥 INI DEBUG WAJIB
//     if (data.startsWith("TIME:")) {
//       rtcTime = data.substring(5);
//     }
//     else if (data.startsWith("ALARM:")) {
//       rtcAlarm = data.substring(6);
//     }
//     else if (data.startsWith("ALARMON:")) {
//       rtcAlarmOn = data.substring(8).toInt();
//     }
//   }
// }

// #endif

#ifndef KOMUNIKASI_ATMEGA_H
#define KOMUNIKASI_ATMEGA_H

// ── Data dari ATmega (diupdate via UART) ──────────────────────
String  rtcTime    = "--:--:--";
String  rtcDate    = "--/--/--";
String  rtcAlarm   = "--:--";
bool    rtcAlarmOn = false;
uint8_t rtcMode    = 0;    // mode lampu ATmega (0-11)
uint8_t rtcTone    = 1;    // nada alarm (1-5)

// ── Baca UART dari ATmega ─────────────────────────────────────
// Format yang dikirim ATmega:
//   TIME:HH:MM:SS
//   DATE:DD/MM/YY
//   ALARM:HH:MM
//   ALARMON:0  atau  ALARMON:1
//   MODE:0
//   TONE:1
void readUART_ATMEGA() {
  while (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    if (data.length() == 0) continue;

    if      (data.startsWith("TIME:"))    rtcTime    = data.substring(5);
    else if (data.startsWith("DATE:"))    rtcDate    = data.substring(5);
    else if (data.startsWith("ALARM:"))   rtcAlarm   = data.substring(6);
    else if (data.startsWith("ALARMON:")) rtcAlarmOn = (data.substring(8).toInt() == 1);
    else if (data.startsWith("MODE:"))    rtcMode    = (uint8_t)data.substring(5).toInt();
    else if (data.startsWith("TONE:"))    rtcTone    = (uint8_t)data.substring(5).toInt();

    Serial.println("[ATmega] " + data); // debug
  }
}

// ── Kirim perintah ke ATmega ──────────────────────────────────
// void sendToATmega(String cmd) {
//   Serial2.println(cmd);
//   Serial.println("[->ATmega] " + cmd);
// }

// // ── JSON status ATmega untuk /api/rtc ────────────────────────
// String buildRtcJson() {
//   String j = "{";
//   j += "\"time\":\""    + rtcTime    + "\",";
//   j += "\"date\":\""    + rtcDate    + "\",";
//   j += "\"alarm\":\""   + rtcAlarm   + "\",";
//   j += "\"alarmon\":"   + String(rtcAlarmOn ? "true" : "false") + ",";
//   j += "\"mode\":"      + String(rtcMode) + ",";
//   j += "\"tone\":"      + String(rtcTone);
//   j += "}";
//   return j;
// }

#endif