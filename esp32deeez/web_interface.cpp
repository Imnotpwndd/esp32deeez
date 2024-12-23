#include <WiFi.h>
#include <WebServer.h>

// Create a WebServer object on port 80
WebServer server(80);

void handle_root() {
  String html = "<html><body>";
  html += "<h1>WiFi Networks</h1>";

  // Start Wi-Fi scan
  int num_networks = WiFi.scanNetworks();

  // Loop through all the networks found
  for (int i = 0; i < num_networks; i++) {
    String encryption = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Encrypted";
    html += "<p>SSID: " + WiFi.SSID(i) + "<br>";
    html += "Encryption: " + encryption + "<br>";
    html += "Signal Strength: " + String(WiFi.RSSI(i)) + " dBm</p>";
  }

  html += "</body></html>";

  // Send the HTML response to the client
  server.send(200, "text/html", html);
}

void setup() {
  // Start the serial monitor for debugging
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin("your-SSID", "your-password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Define the root page handler
  server.on("/", HTTP_GET, handle_root);

  // Start the server
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
