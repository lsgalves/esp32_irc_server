#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include "logo.h"
#include "minilogo.h"
#include "check.h"
#include "error.h"

#define APP_NAME "example"
#define ADMIN_PASS "<PUT-YOUR-ADMIN-PASSWORD>"
#define WIFI_SSID "<PUT-YOUR-SSID>"
#define WIFI_PASS "<PUT-YOUR-WIFI-PASSWORD>"
#define IRC_PORT 6667

#define DISPLAY_W 320
#define DISPLAY_H 170

#define APP_BLACK 0x21CE
#define APP_WHITE 0xF7DF
#define APP_GRAY 0x94B3
#define APP_GREEN 0x1523
#define APP_RED 0xD0E3
#define APP_LGREEN 0xE77C
#define APP_LRED 0xF6FB

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite logoSprite = TFT_eSprite(&tft);
TFT_eSprite checkSprite = TFT_eSprite(&tft);

WiFiServer server(IRC_PORT);

int logoX = 106;
int pulse = 7;
int pulseClean = 0;
int clients = 0;
String channels[] = {"example"};

String getMemoryUsage() {
  size_t totalHeapSize = esp_get_minimum_free_heap_size() + esp_get_free_heap_size();
  size_t freeHeapSize = esp_get_free_heap_size();
  float percMemUsage = 100 - (((float)freeHeapSize / (float)totalHeapSize) * 100);
  static char memUsage[10];
  snprintf(memUsage, sizeof(memUsage), "%.2f%%", percMemUsage);
  return memUsage;
}

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  server.begin();
  Serial.println("Server started");
}

void splashScreen() {
  background.createSprite(DISPLAY_W, DISPLAY_H);
  background.setSwapBytes(true);
  logoSprite.createSprite(109, 63);
  background.fillSprite(TFT_WHITE);
  logoSprite.pushImage(0, 0, 109, 63, logo);
  logoSprite.pushToSprite(&background, logoX, 54, TFT_WHITE);
  background.pushSprite(0, 0);
  delay(2000);

  while (logoX > 25) {
    logoX-=3;
    background.fillSprite(TFT_WHITE);
    logoSprite.pushToSprite(&background, logoX, 54, TFT_WHITE);
    background.pushSprite(0, 0);
  }
  delay(1000);

  int nameLength = strlen(APP_NAME);
  char nameBuffer[nameLength + 1];
  nameBuffer[0] = '\0';

  tft.setTextColor(APP_BLACK, APP_BLACK);
  for (int i = 0; i < nameLength; i++) {
    strncat(nameBuffer, &APP_NAME[i], 1);
    tft.drawString(nameBuffer, 140, 70, 4);
    delay(300);
  }
  tft.drawString("IRC SERVER", 181, 95, 2);
}

void dashboard(){
  String ip = "0.0.0.0";
  int conn = 0;
  if (WiFi.status() == WL_CONNECTED) {
    conn = 1;
    ip = WiFi.localIP().toString();
  }

  background.createSprite(DISPLAY_W, DISPLAY_H);
  background.setSwapBytes(true);
  background.fillSprite(APP_WHITE);

  if (pulse < 18) {
    pulse++;
  } else if (pulseClean < 18) {
    pulseClean++;
  } else {
    pulse = 7;
    pulseClean = 0;
  }
  uint16_t pulseColor = conn ? APP_LGREEN : APP_LRED;
  uint16_t connColor = conn ? APP_GREEN : APP_RED;

  background.fillCircle(23, 23, pulse, pulseColor);
  background.fillCircle(23, 23, pulseClean, APP_WHITE);
  checkSprite.createSprite(20, 20);
  checkSprite.pushImage(0, 0, 20, 20, conn ? check : error);
  checkSprite.pushToSprite(&background, 13, 13, TFT_BLACK);

  background.setTextColor(TFT_BLACK, TFT_BLACK);
  background.drawString(ip, 46, 10, 4);
  background.setTextColor(connColor, connColor);
  background.drawString(conn ? "Current server IP" : "Server without connection", 48, 32, 1);

  logoSprite.createSprite(41, 24);
  logoSprite.pushImage(0, 0, 41, 24, minilogo);
  logoSprite.pushToSprite(&background, 266, 13, TFT_BLACK);

  String printChannels = "";
  int chann_size = sizeof(channels) / sizeof(channels[0]);
  for (int i = 0; i < chann_size; i++) {
    printChannels += "#" + channels[i] + (chann_size - 1 == i ? "" : ", ");
  }

  background.setTextColor(APP_GRAY, APP_GRAY);
  background.drawString("SSID", 10, 56, 2);
  background.drawString("Port", 10, 73, 2);
  background.drawString("Memory", 10, 90, 2);
  background.drawString("Clients", 10, 107, 2);
  background.drawString("Channels", 10, 124, 2);
  background.setTextColor(TFT_BLACK, TFT_BLACK);
  background.drawString(WIFI_SSID, 70, 56, 2);
  background.drawString(String(IRC_PORT), 70, 73, 2);
  background.drawString(getMemoryUsage(), 70, 90, 2);
  background.drawString(String(clients), 70, 107, 2);
  background.drawString(printChannels, 70, 124, 2);

  background.pushSprite(0,0);
}

void setup(void) {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);

  splashScreen();
  connectWifi();
  delay(2000);
  logoSprite.deleteSprite();
  background.deleteSprite();
}

void loop() {
  dashboard();
  WiFiClient client = server.available();
  if (client) {
    clients++;
    Serial.println("New client connected");
    // TODO: Implement IRC server
    client.stop();
    Serial.println("Client disconnected");
  }
  delay(100);
}
