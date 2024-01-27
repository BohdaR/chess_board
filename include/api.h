#ifndef API_H
#define API_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include "constants.h"

String HOST_NAME = "http://176.117.186.180:3000";

void connectToWiFi() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(SSID, PASSWORD);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

JsonDocument toJson(String jsonString) {
    // This function converts JSON string to JsonDocument object
    // Create a JSON object
    JsonDocument jsonDoc;

    // Parse the JSON string
    deserializeJson(jsonDoc, jsonString);
    return jsonDoc;
}

String post(const String url, const String postData) {
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
    String url = HOST_NAME + "/create_game";
    String response = post(url, "");
    setGameId(response);
}

void sendMoveTask(void *parameter) {
    // Create JSON document
    JsonDocument jsonDoc;
    jsonDoc["game_id"] = GAME_ID;
    jsonDoc["move"] = currentMove;
    jsonDoc["move_side"] = PositionDynamics.whoseMove;

    // Serialize JSON to string
    String jsonData;
    serializeJson(jsonDoc, jsonData);

    // Perform HTTP POST
    post(HOST_NAME + "/write_move", jsonData);

    // Delete the task when done
    vTaskDelete(NULL);
}

// Function to trigger the move sending task
void sendMove(String move) {
    // Set current move
    currentMove = move;
    xTaskCreate(
        sendMoveTask,       // Function to run
        "SendMoveTask",     // Task name
        8192,               // Stack size (bytes)
        NULL,               // Task parameters (pointer to the move string)
        1,                  // Priority
        NULL                // Task handle
    );
}

#endif
