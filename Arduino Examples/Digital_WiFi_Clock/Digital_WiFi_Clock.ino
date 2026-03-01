#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h>
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
TFT_eSPI tft = TFT_eSPI();

const char* ssid = "Capgemini_4G";
const char* password = "MN704116";

const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 19800;   // India
int daylightOffset_sec = 0;

int prev_sec = -1;

void setup() {
  Serial.begin(115200);

  k10.begin();          // IMPORTANT for K10 power & screen
  k10.initScreen(2);    // rotation

  tft.init();           // TFT init AFTER k10.initScreen
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.print("Connecting WiFi...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  tft.fillScreen(TFT_BLACK);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

int prev_minute = -1;

void loop() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return;
  }

  if (timeinfo.tm_min != prev_minute) {

    prev_minute = timeinfo.tm_min;

    tft.fillScreen(TFT_BLACK);

    int hour24 = timeinfo.tm_hour;
    int hour12;
    String ampm;

    if (hour24 == 0) {
      hour12 = 12;
      ampm = "AM";
    } else if (hour24 < 12) {
      hour12 = hour24;
      ampm = "AM";
    } else if (hour24 == 12) {
      hour12 = 12;
      ampm = "PM";
    } else {
      hour12 = hour24 - 12;
      ampm = "PM";
    }

    // ===== TIME STRING =====
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", hour12, timeinfo.tm_min);

    // Calculate widths (default font approx 6px wide per char)
    int timeWidth = strlen(timeStr) * 6 * 6;   // textSize 6
    int ampmWidth = ampm.length() * 6 * 3;     // textSize 3
    int gap = 10;

    int totalWidth = timeWidth + gap + ampmWidth;
    int startX = (320 - totalWidth) / 2;

    // ===== DRAW TIME =====
    tft.setTextSize(6);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(startX, 40);
    tft.print(timeStr);

    // ===== DRAW AM/PM (beside time) =====
    tft.setTextSize(3);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(startX + timeWidth + gap, 60);
    tft.print(ampm);

    // ===== DAY =====
    char dayStr[12];
    strftime(dayStr, sizeof(dayStr), "%A", &timeinfo);

    int dayWidth = strlen(dayStr) * 6 * 3;
    int dayX = (320 - dayWidth) / 2;

    tft.setTextSize(3);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(dayX, 120);
    tft.print(dayStr);

    // ===== DATE (DD-MMM-YYYY) =====
    char dateStr[15];
    strftime(dateStr, sizeof(dateStr), "%d-%b-%Y", &timeinfo);

    int dateWidth = strlen(dateStr) * 6 * 3;
    int dateX = (320 - dateWidth) / 2;

    tft.setTextSize(3);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(dateX, 155);
    tft.print(dateStr);
  }

  delay(500);
}