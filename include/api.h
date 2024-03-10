#ifndef API_H
#define API_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "Arduino.h"
#include "constants.h"

String HOST_NAME = "http://176.117.186.180:3000";

void connectToWiFi() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(SSID, PASSWORD);
    Serial.println("\nConnecting");

    // Try to connect to the Wi-Fi
    for(int i = 0; i < 30; i++) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            delay(100);
        } else {
            CONNECTED_TO_INTERNET = true;
            Serial.println("\nConnected to the WiFi network");
            Serial.print("Local ESP32 IP: ");
            Serial.println(WiFi.localIP());
            return;
        }
    }
    CONNECTED_TO_INTERNET = false;
}

JsonDocument toJson(String jsonString) {
    // This function converts JSON string to JsonDocument object
    // Create a JSON object
    JsonDocument jsonDoc;

    // Parse the JSON string
    deserializeJson(jsonDoc, jsonString);
    return jsonDoc;
}

String post(const String url, const String postData, int attempt = 0) {
    if (attempt == MAX_ATTEMPT_NUMBER) return String();

    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    }
    // Create an HTTPClient object
    HTTPClient http;

    // Specify the target URL
    http.begin(url);

    // Set the content type
    http.addHeader("Content-Type", "application/json");

    // Perform the POST request
    int httpResponseCode = http.POST(postData);

    // Check for a successful response
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Get the response payload
        String response = http.getString();

        // Close the connection
        http.end();

        return response;
    }
    else {
        Serial.print("Error on HTTP request. Response code: ");
        Serial.println(httpResponseCode);

        // Close the connection
        http.end();
        post(url, postData, attempt + 1);
        return String();
    }
}

void setGameId(const String response) {
    // Create a JSON object
    JsonDocument jsonDoc;  // Adjust the size based on your JSON data

    // Parse the JSON string
    deserializeJson(jsonDoc, response);

    // Extract the "game_id" from the JSON object
    GAME_ID = toJson(response)["game_id"];
}

void createNewGame() {
    String url = HOST_NAME + "/chess_games";
    String response = post(url, "");
    setGameId(response);
}

void sendMoveTask(void *parameter) {
    if (!CONNECTED_TO_INTERNET || !GAME_ID) return;

    // Cast the parameter to the correct type
    String* jsonDataPtr = (String*)parameter;

    // Get the JSON data from the pointer
    String jsonData = *jsonDataPtr;

    // Perform HTTP POST
    post(HOST_NAME + "/chess_moves", jsonData);

    // Free the dynamically allocated memory
    delete jsonDataPtr;

    // Delete the task when done
    vTaskDelete(NULL);
}

// Function to trigger the move sending task
void sendMove(String moveNotation, int fromSquare, int toSquare, bool castling) {
    if (!CONNECTED_TO_INTERNET || !GAME_ID) return;

    // Set current move
    JsonDocument currentMove;
    currentMove["chess_game_id"] = GAME_ID;
    currentMove["notation"] = moveNotation;
    currentMove["from_square"] = fromSquare;
    currentMove["to_square"] = toSquare;
    currentMove["move_side"] = PositionDynamics.whoseMove;
    currentMove["castling"] = castling;

    // Serialize JSON to string
    String* jsonDataPtr = new String();
    serializeJson(currentMove, *jsonDataPtr);

    xTaskCreate(
        sendMoveTask,       // Function to run
        "SendMoveTask",     // Task name
        8192,               // Stack size (bytes)
        (void*)jsonDataPtr, // Task parameters (pointer to the move string)
        moveNumber,         // Priority
        NULL                // Task handle
    );
}

#endif
