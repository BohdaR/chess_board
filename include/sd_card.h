#ifndef SD_CARD_H
#define SD_CARD_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "constants.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, String path, String message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, String path, String message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void createNewPGN(fs::FS &fs) {
    // Open the directory
    File directory = fs.open("/games");
    if (!directory) {
        Serial.println("Error opening directory!");
        return;
    }

    int numFiles = 1;

    // Loop through all files in the directory
    while (true) {
        File file = directory.openNextFile();
        if (!file) {
            // No more files
            break;
        }

        // Check if it's a regular file
        if (file.isDirectory()) {
            // Skip directories
            continue;
        }

        // Increment the file count
        numFiles++;

        file.close();
    }

    FILE_NAME = "/games/";
    FILE_NAME += String(numFiles);
    FILE_NAME += ".txt";

    // Close the directory
    directory.close();

    // Create the new file
    File newFile = SD.open(FILE_NAME, FILE_WRITE);
    if (!newFile) {
        Serial.println("Error creating new file!");
        return;
    }

    // Close the new file
    newFile.close();

    // Print the number of files
    Serial.print("New file name: ");
    Serial.println(FILE_NAME);
}

void writeMoveToPGN(String move) {
    if (moveNumber % 2 == 0) {
        move = String((moveNumber / 2) + 1) + ". " + move + " ";
        appendFile(SD, FILE_NAME, move);
    } else {
        move += "\n";
        appendFile(SD, FILE_NAME, move);
    }
}

#endif
