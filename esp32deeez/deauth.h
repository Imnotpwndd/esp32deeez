#ifndef DEAUTH_H
#define DEAUTH_H

// Include necessary libraries
#include <Arduino.h>

// Define the deauth_frame_t structure
typedef struct {
  uint8_t frame_control[2];       // Frame control bytes (typically for management frames)
  uint8_t duration[2];            // Duration field
  uint8_t station[6];             // MAC address of the target station
  uint8_t sender[6];              // MAC address of the sender
  uint8_t access_point[6];        // MAC address of the access point
  uint8_t fragment_sequence[2];   // Fragmentation control
  uint16_t reason;                // Deauthentication reason code
} deauth_frame_t;

// Declare the function to send deauth frames
void send_deauth_frame(deauth_frame_t deauth_frame);

// Declare the function to start the deauthentication attack
void start_deauth(int wifi_number, int attack_type, uint16_t reason);

// Declare the function to stop the deauthentication attack
void stop_deauth();

// Declare the function for sniffer callback
IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);

#endif // DEAUTH_H
