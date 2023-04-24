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
TFT_eSprite logo_sprite = TFT_eSprite(&tft);
TFT_eSprite check_sprite = TFT_eSprite(&tft);

WiFiServer server(IRC_PORT);

int logo_x = 106;
int pulse = 7;
int pulse_clean = 0;
int clients = 0;
String channels[] = {"example"};

String get_memory_usage() {
  size_t total_heap_size = esp_get_minimum_free_heap_size() + esp_get_free_heap_size();
  size_t free_heap_size = esp_get_free_heap_size();
  float perc_cem_usage = 100 - (((float)free_heap_size / (float)total_heap_size) * 100);
  static char mem_usage[10];
  snprintf(mem_usage, sizeof(mem_usage), "%.2f%%", perc_cem_usage);
  return mem_usage;
}

void wifi_connect() {
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

void splash_screen() {
  background.createSprite(DISPLAY_W, DISPLAY_H);
  background.setSwapBytes(true);
  logo_sprite.createSprite(109, 63);
  background.fillSprite(TFT_WHITE);
  logo_sprite.pushImage(0, 0, 109, 63, logo);
  logo_sprite.pushToSprite(&background, logo_x, 54, TFT_WHITE);
  background.pushSprite(0, 0);
  delay(2000);

  while (logo_x > 25) {
    logo_x -= 3;
    background.fillSprite(TFT_WHITE);
    logo_sprite.pushToSprite(&background, logo_x, 54, TFT_WHITE);
    background.pushSprite(0, 0);
  }
  delay(1000);

  int name_length = strlen(APP_NAME);
  char name_buffer[name_length + 1];
  name_buffer[0] = '\0';

  tft.setTextColor(APP_BLACK, APP_BLACK);
  for (int i = 0; i < name_length; i++) {
    strncat(name_buffer, &APP_NAME[i], 1);
    tft.drawString(name_buffer, 140, 70, 4);
    delay(300);
  }
  tft.drawString("IRC SERVER", 181, 95, 2);
}

void dashboard() {
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
  } else if (pulse_clean < 18) {
    pulse_clean++;
  } else {
    pulse = 7;
    pulse_clean = 0;
  }
  uint16_t pulse_color = conn ? APP_LGREEN : APP_LRED;
  uint16_t conn_color = conn ? APP_GREEN : APP_RED;

  background.fillCircle(23, 23, pulse, pulse_color);
  background.fillCircle(23, 23, pulse_clean, APP_WHITE);
  check_sprite.createSprite(20, 20);
  check_sprite.pushImage(0, 0, 20, 20, conn ? check : error);
  check_sprite.pushToSprite(&background, 13, 13, TFT_BLACK);

  background.setTextColor(TFT_BLACK, TFT_BLACK);
  background.drawString(ip, 46, 10, 4);
  background.setTextColor(conn_color, conn_color);
  background.drawString(conn ? "Current server IP" : "Server without connection", 48, 32, 1);

  logo_sprite.createSprite(41, 24);
  logo_sprite.pushImage(0, 0, 41, 24, minilogo);
  logo_sprite.pushToSprite(&background, 266, 13, TFT_BLACK);

  String print_channels = "";
  int chann_size = sizeof(channels) / sizeof(channels[0]);
  for (int i = 0; i < chann_size; i++) {
    print_channels += "#" + channels[i] + (chann_size - 1 == i ? "" : ", ");
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
  background.drawString(get_memory_usage(), 70, 90, 2);
  background.drawString(String(clients), 70, 107, 2);
  background.drawString(print_channels, 70, 124, 2);

  background.pushSprite(0,0);
}

void setup(void) {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);

  splash_screen();
  wifi_connect();
  delay(2000);
  logo_sprite.deleteSprite();
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
