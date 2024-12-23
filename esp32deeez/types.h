#ifndef TYPES_H
#define TYPES_H

#include "deauth.h"  // Include deauth.h to get the deauth_frame_t definition

// Define other structures as needed
typedef struct {
  uint16_t frame_ctrl;            // Frame control field
  uint16_t duration;              // Duration field
  uint8_t dest[6];                // Destination MAC address
  uint8_t src[6];                 // Source MAC address
  uint8_t bssid[6];               // BSSID (AP's MAC address)
  uint16_t sequence_ctrl;         // Sequence control field
  uint8_t addr4[6];               // Address 4 (optional, used in some frames like QoS)
} mac_hdr_t;

// Define the wifi_packet_t structure
typedef struct {
  mac_hdr_t hdr;                  // MAC header
  uint8_t payload[0];             // Payload (of variable length)
} wifi_packet_t;

#endif // TYPES_H
