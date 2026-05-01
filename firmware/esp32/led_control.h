#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <WS2812FX.h>
#include <Preferences.h>

#define LED_COUNT 25
#define LED_PIN 14
#define SEARCH_DURATION 10000
#define SEARCH_SPEED 80

WS2812FX ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct LedState {
  bool on = true;
  uint8_t brightness = 128;
  uint32_t color = 0xFFFFFF;
  uint8_t fx = FX_MODE_BREATH;
  uint8_t sx = 128;
} ledState;

bool isSearching = false;
unsigned long searchStartTime = 0;

void saveLedState() {
  Preferences lp;
  lp.begin("ledstate", false);
  lp.putBool("on", ledState.on);
  lp.putUChar("bri", ledState.brightness);
  lp.putUInt("col", ledState.color);
  lp.putUChar("fx", ledState.fx);
  lp.putUChar("sx", ledState.sx);
  lp.end();
}

void loadLedState() {
  Preferences lp;
  lp.begin("ledstate", true);
  ledState.on = lp.getBool("on", true);
  ledState.brightness = lp.getUChar("bri", 128);
  ledState.color = lp.getUInt("col", 0xFFFFFF);
  ledState.fx = lp.getUChar("fx", FX_MODE_BREATH);
  ledState.sx = lp.getUChar("sx", 128);
  lp.end();
}

void applyNormalState() {
  ws2812fx.resetSegments();

  if (!ledState.on) {
    ws2812fx.stop();
    ws2812fx.setBrightness(0);
    ws2812fx.trigger();
    return;
  }

  ws2812fx.setSegment(
    0,
    (uint16_t)0,
    (uint16_t)(LED_COUNT - 1),
    (uint8_t)ledState.fx,
    (uint32_t)ledState.color,
    (uint16_t)map(ledState.sx, 1, 255, 5000, 10),
    false
  );

  ws2812fx.setBrightness(ledState.brightness);
  ws2812fx.start();
  ws2812fx.trigger();
}

void setupLED() {
  loadLedState();
  ws2812fx.init();
  applyNormalState();
}

String buildStateJson() {
  uint8_t r = (ledState.color >> 16) & 0xFF;
  uint8_t g = (ledState.color >> 8) & 0xFF;
  uint8_t b = ledState.color & 0xFF;

  String j = "{\"on\":" + String(ledState.on ? "true" : "false");
  j += ",\"bri\":" + String(ledState.brightness);
  j += ",\"seg\":[{\"fx\":" + String(ledState.fx);
  j += ",\"sx\":" + String(ledState.sx);
  j += ",\"col\":[[" + String(r) + "," + String(g) + "," + String(b) + "]]}]}";

  return j;
}

void handleLED(JsonObject root) {
  bool changed = false;

  if (root.containsKey("on")) {
    ledState.on = root["on"].as<bool>();
    changed = true;
  }

  if (root.containsKey("bri")) {
    ledState.brightness = root["bri"].as<uint8_t>();
    changed = true;
  }

  JsonArray seg = root["seg"];
  if (!seg.isNull() && seg.size() > 0) {
    JsonObject s = seg[0];

    // 🔍 SEARCH LACI
    if (s.containsKey("start")) {
      uint16_t st = s["start"].as<uint16_t>();
      uint16_t sp = s["stop"].as<uint16_t>();

      isSearching = true;
      searchStartTime = millis();

      ws2812fx.resetSegments();
      ws2812fx.setBrightness(ledState.on ? ledState.brightness : 0);

      ws2812fx.setSegment(
        0,
        st,
        (uint16_t)(sp - 1),
        (uint8_t)FX_MODE_BLINK,
        (uint32_t)ledState.color,
        (uint16_t)SEARCH_SPEED,
        false
      );

      if (st > 0) {
        ws2812fx.setSegment(
          1,
          0,
          (uint16_t)(st - 1),
          (uint8_t)FX_MODE_STATIC,
          (uint32_t)0,
          (uint16_t)1000,
          false
        );
      }

      if (sp < LED_COUNT) {
        ws2812fx.setSegment(
          2,
          sp,
          (uint16_t)(LED_COUNT - 1),
          (uint8_t)FX_MODE_STATIC,
          (uint32_t)0,
          (uint16_t)1000,
          false
        );
      }

      ws2812fx.start();
      ws2812fx.trigger();
      return;
    }

    if (s.containsKey("col")) {
      JsonArray c = s["col"][0];
      uint8_t r = c[0];
      uint8_t g = c[1];
      uint8_t b = c[2];
      ledState.color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
      changed = true;
    }

    if (s.containsKey("fx")) {
      ledState.fx = s["fx"].as<uint8_t>();
      changed = true;
    }

    if (s.containsKey("sx")) {
      ledState.sx = s["sx"].as<uint8_t>();
      changed = true;
    }
  }

  if (changed) {
    applyNormalState();
    saveLedState();
  }
}

void updateSearchStatus() {
  if (!isSearching) return;

  if (millis() - searchStartTime > SEARCH_DURATION) {
    isSearching = false;
    applyNormalState();
  }
}

#endif