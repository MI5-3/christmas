#include "Constatns.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* host = HOST_NAME;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

uint8_t op_mode = OP_MODE_SOLID;
unsigned long op_mode_change_time = millis();

uint8_t op_mode_chase_length = 6;
int op_mode_chase_position = -6;
unsigned long op_mode_chase_change_time = millis();

unsigned long op_mode_random_change_time = millis();
uint8_t op_mode_random_r = 0;
uint8_t op_mode_random_g = 0;
uint8_t op_mode_random_b = 0;

uint8_t op_mode_color_wipe_position = 0;
unsigned long op_mode_color_wipe_change_time = millis();

uint8_t op_mode_shift_position = 0;
unsigned long op_mode_shift_change_time = millis();

uint8_t op_mode_solid_r = 0;
uint8_t op_mode_solid_g = 0;
uint8_t op_mode_solid_b = 0;
uint8_t op_mode_solid_random = 0;
unsigned long op_mode_solid_change_time = millis();

void setup();
void loop();

void op_mode_chase();
void op_mode_color_wipe();
void op_mode_shift();
void op_mode_solid();
void op_mode_random();