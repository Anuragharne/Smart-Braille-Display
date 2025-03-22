#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <esp_task_wdt.h>


// WiFi credentials
const char* ssid = "Astro";
const char* password = "@nuragH006";

// Create AsyncWebServer on port 80
AsyncWebServer server(80);

// Define servo motors
Servo servos[6];
const int servoPins[6] = {12, 13, 14, 15, 2, 4};

// Braille Dictionary
const int brailleMap[26][6] = {
    {1, 0, 0, 0, 0, 0}, // A
    {1, 1, 0, 0, 0, 0}, // B
    {1, 0, 0, 1, 0, 0}, // C
    {1, 0, 0, 1, 1, 0}, // D
    {1, 0, 0, 0, 1, 0}, // E
    {1, 1, 0, 1, 0, 0}, // F
    {1, 1, 0, 1, 1, 0}, // G
    {1, 1, 0, 0, 1, 0}, // H
    {0, 1, 0, 1, 0, 0}, // I
    {0, 1, 0, 1, 1, 0}, // J
    {1, 0, 1, 0, 0, 0}, // K
    {1, 1, 1, 0, 0, 0}, // L
    {1, 0, 1, 1, 0, 0}, // M
    {1, 0, 1, 1, 1, 0}, // N
    {1, 0, 1, 0, 1, 0}, // O
    {1, 1, 1, 1, 0, 0}, // P
    {1, 1, 1, 1, 1, 0}, // Q
    {1, 1, 1, 0, 1, 0}, // R
    {0, 1, 1, 1, 0, 0}, // S
    {0, 1, 1, 1, 1, 0}, // T
    {1, 0, 1, 0, 0, 1}, // U
    {1, 1, 1, 0, 0, 1}, // V
    {0, 1, 0, 1, 1, 1}, // W
    {1, 0, 1, 1, 0, 1}, // X
    {1, 0, 1, 1, 1, 1}, // Y
    {1, 0, 1, 0, 1, 1}  // Z
};

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Attach servos to pins
    for (int i = 0; i < 6; i++) {
        servos[i].attach(servoPins[i],500,2400);
        servos[i].write(20);  // Set all servos to rest position initially
    }
    
    // Web page to input text
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<html><body>"
                      "<h2>Enter Text for Braille</h2>"
                      "<form action='/send' method='POST'>"
                      "<input type='text' name='text' required>"
                      "<input type='submit' value='Send'>"
                      "</form></body></html>";
        request->send(200, "text/html", html);
    });
    
    // Handle text submission
    server.on("/send", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("text", true)) {
            String receivedText = request->getParam("text", true)->value();
            Serial.println("Received: " + receivedText);
            processText(receivedText);
        }
        request->send(200, "text/plain", "Text received");
    });

    server.begin();
}

void loop() {
    // Nothing in loop, everything handled asynchronously
}

void processText(String text) {
    text.toUpperCase(); // Convert all to uppercase for uniformity
    
    for (int i = 0; i < text.length(); i++) {
        char letter = text[i];
        Serial.print("Processing letter: ");
        Serial.println(letter);
        
        if (letter >= 'A' && letter <= 'Z') {
            int index = letter - 'A';
            displayBraille(brailleMap[index]);
        } else {
            Serial.println("Unsupported character, skipping...");
        }
        
        esp_task_wdt_reset();
        
        delay(1000); // Small delay to display the letter
        
        resetServos(); // Reset servos after displaying each letter
    }
}

void displayBraille(const int pattern[6]) {
    Serial.print("Braille Pattern: ");
    
    for (int i = 0; i < 6; i++) {
        Serial.print(pattern[i]);
        if (pattern[i] == 1) {
            servos[i].write(0);
            delay(200);  // Move up
        } else {
            servos[i].write(20);
            delay(200); // Move down
        }
    }
    Serial.println();
}

// Function to reset all servos to rest position (90°)
void resetServos() {
    delay(500);  // Small delay before resetting for better visibility
    Serial.println("Resetting servos to rest position...");
    for (int i = 0; i < 6; i++) {
        servos[i].write(20);  // Move all servos to rest position
    }
}
