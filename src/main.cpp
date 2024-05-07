

#include <Arduino.h>
#include <Keyboard.h>

#include "pinDefinitions.h"

// https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/

byte read8BitShiftReg();
void printByte(byte, String);
void updateKey(bool pressed, unsigned char key);

void setup() {
    // Begin serial communication.
    Serial.begin(9600);

    // Define pin modes.
    pinMode(DATA_PIN, INPUT);
    pinMode(LOAD_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    Keyboard.begin();
}

void loop() {
    // Read the data from the shift register.
    byte data = read8BitShiftReg();

    // For the data received to be 0, either all buttons are pressed (which is
    // not possible on the NES controller) or the controller is not connected.
    bool controllerConnected = !(data == 0);

    // TODO Make the criteria fpr the controller being disconnected "Up and down
    // buttons pressed at the same time, or left and right buttons pressed at
    // the same time."

    // Print the received data to the serial port.
    printByte(data, ",");

    Serial.print(" controllerConnected: ");
    Serial.println(controllerConnected);
    Serial.println();

    // Break the byte into its individual bits.
    // The values are inverted because the buttons are pulled low when pressed.
    bool button_a = (!(data & 128) && controllerConnected);
    bool button_b = (!(data & 64) && controllerConnected);
    bool button_select = (!(data & 32) && controllerConnected);
    bool button_start = (!(data & 16) && controllerConnected);
    bool button_up = (!(data & 8) && controllerConnected);
    bool button_down = (!(data & 4) && controllerConnected);
    bool button_left = (!(data & 2) && controllerConnected);
    bool button_right = (!(data & 1) && controllerConnected);

    // This is the configuration for the game Bit Blaster XL
    updateKey(button_a, KEY_LEFT_CTRL);
    updateKey(button_b, KEY_LEFT_ALT);
    updateKey(button_select, KEY_ESC);
    updateKey(button_start, KEY_RETURN);
    updateKey(button_up, KEY_UP_ARROW);
    updateKey(button_down, KEY_DOWN_ARROW);
    updateKey(button_left, KEY_LEFT_ARROW);
    updateKey(button_right, KEY_RIGHT_ARROW);
}

/**
 * @brief Reads the data from the a 8-bit shift register.
 *
 * @return (byte) The data from the a 8-bit shift register.
 */
byte read8BitShiftReg() {
    // Create a byte for storing the received data from the shift register.
    byte shiftRegisterContents = 0;

    // Load the data into the shift register by sending a pulse to the load
    // pin.
    digitalWrite(LOAD_PIN, LOW);
    delayMicroseconds(10);
    digitalWrite(LOAD_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(LOAD_PIN, LOW);

    // For of the 8 bits in a byte.
    for (int i = 0; i < 8; i++) {
        // Shift the received data up by one.
        shiftRegisterContents <<= 1;

        // Load the available bit from the shift register into the lsb of
        // the received data byte.
        shiftRegisterContents |= digitalRead(DATA_PIN);

        // Pulse the clock pin to shift the data inside the shift register.
        digitalWrite(CLOCK_PIN, LOW);
        delayMicroseconds(10);
        digitalWrite(CLOCK_PIN, HIGH);
        delayMicroseconds(10);
    }
    return shiftRegisterContents;
}

/**
 * @brief Prints a byte of data as its individual bits.
 *
 * This function takes a byte and prints it to the serial port as its
 * individual 8 bits, starting with the most significant bit on the left.
 * Any leading zeros of the data are also printed. A separator can also be
 * provided to separate the bits, but is not required.
 *
 * for example,printByte(13) would print 00001101. and printByte(13,",")
 * would print 0,0,0,0,1,1,0,1.
 *
 * This function assumes that a serial connection has been initialised.
 *
 * @param byteToPrint The unsigned 8 bits of data to print to the serial
 * port.
 * @param separator The string to be printed between each bit.
 */
void printByte(byte byteToPrint, String separator = "e") {
    // Print the 7 leading bits of the byte, followed by the optional
    // separator.
    for (byte b = 7; b != 0; b--) {
        Serial.print((byteToPrint >> b) & 1);
        Serial.print(separator);
    }
    // Print the final bit, followed by a newline character.
    Serial.println(byteToPrint & 1);
}

/**
 * @brief Updates the state of a key on the keyboard.
 *
 * @param key The key to update, as defined in the Keyboard library.
 * @param pressed Whether the key is pressed or released.
 */
void updateKey(bool pressed, unsigned char key) {
    if (pressed) {
        Keyboard.press(key);
    } else {
        Keyboard.release(key);
    }
}
