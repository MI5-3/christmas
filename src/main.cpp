#include "main.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Booting ...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA.setHostname(host);

    ArduinoOTA.onStart([]() {
        Serial.println("Start updating ");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();

    strip.begin();
    strip.show();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    ArduinoOTA.handle();

    switch (op_mode) {
        case OP_MODE_CHASE:
            op_mode_chase();
            break;
        case OP_MODE_RCHASE:
            op_mode_rchase();
            break;
        case OP_MODE_COLOR_WIPE:
            op_mode_color_wipe();
            break;
        case OP_MODE_SHIFT:
            op_mode_shift();
            break;
        case OP_MODE_RANDOM:
            op_mode_random();
            break;
        case OP_MODE_SOLID:
            op_mode_solid();
            break;
    }

    strip.show();

    if ((unsigned long) (millis() - op_mode_change_time) > OP_MODE_TIME) {
        op_mode_change_time = millis();
        op_mode = random(0, 5);

        op_mode_chase_position = -6;
        op_mode_color_wipe_position = 0;
        op_mode_shift_position = 0;
    }
}

void op_mode_chase() {
    if ((unsigned long) (millis() - op_mode_chase_change_time) > OP_MODE_CHASE_TIME) {
        op_mode_chase_change_time = millis();

        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }

        for (int i = op_mode_chase_position; i < op_mode_chase_position + op_mode_chase_length; i++) {
            if (i > 0 && i < NUM_LEDS) {
                strip.setPixelColor(i, strip.Color(0, 255, 0));
            }
            
            if(i >= NUM_LEDS) {
                Serial.println(i - NUM_LEDS);
                strip.setPixelColor(i - NUM_LEDS, strip.Color(0, 255, 0));
            }
        }

        op_mode_chase_position++;

        if (op_mode_chase_position > NUM_LEDS) {
            op_mode_chase_position = 0;
        }
    }
}

void op_mode_rchase() {
    if ((unsigned long) (millis() - op_mode_rchase_change_time) > OP_MODE_CHASE_TIME) {
        op_mode_rchase_change_time = millis();
        
        if(op_mode_rchase_position == 0) {
            //NOTE: we are reusing variables here!
            op_mode_random_color = random(0, OP_MODE_RANDOM_LENGTH);
        }
        
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }

        for (int i = op_mode_rchase_position; i < op_mode_rchase_position + op_mode_rchase_length; i++) {
            if (i > 0 && i < NUM_LEDS) {
                strip.setPixelColor(i, op_mode_random_colors[op_mode_random_color]);
            }
            
            if(i >= NUM_LEDS) {
                Serial.println(i - NUM_LEDS);
                strip.setPixelColor(i - NUM_LEDS, op_mode_random_colors[op_mode_random_color]);
            }
        }

        op_mode_rchase_position++;

        if (op_mode_rchase_position > NUM_LEDS) {
            op_mode_rchase_position = 0;
        }
    }
}

void op_mode_color_wipe() {
    if ((unsigned long) (millis() - op_mode_color_wipe_change_time) > OP_MODE_COLOR_WIPE_TIME) {
        op_mode_color_wipe_change_time = millis();

        strip.setPixelColor(op_mode_color_wipe_position, strip.Color(0, 255, 0));

        op_mode_color_wipe_position++;

        if (op_mode_color_wipe_position > NUM_LEDS) {
            op_mode_color_wipe_position = 0;

            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
        }
    }
}

void op_mode_shift() {
    if ((unsigned long) (millis() - op_mode_shift_change_time) > OP_MODE_SHIFT_TIME) {
        op_mode_shift_change_time = millis();

        if (op_mode_shift_position == 2) {
            op_mode_shift_position = 0;
        }

        for (int i = 0; i < strip.numPixels(); i++) {
            if (i % 3 == 0) {
                strip.setPixelColor(i + op_mode_shift_position, strip.Color(0, 0, 0));
            } else {
                strip.setPixelColor(i + op_mode_shift_position, strip.Color(0, 255, 0));
            }
        }

        op_mode_shift_position++;
    }
}

void op_mode_solid() {
    if ((unsigned long) (millis() - op_mode_solid_change_time) > OP_MODE_SOLID_TIME) {
        op_mode_solid_change_time = millis();
        if (op_mode_solid_state == 0) {
            op_mode_solid_state = 1;
        } else if (op_mode_solid_state == 1) {
            op_mode_solid_state = 0;

            //we need to selec color

            op_mode_solid_random = random(1, 4);

            op_mode_solid_r = 0;
            op_mode_solid_g = 0;
            op_mode_solid_b = 0;

            if (op_mode_solid_random == 1) {
                op_mode_solid_r = 255;
            } else if (op_mode_solid_random == 2) {
                op_mode_solid_g = 255;
            } else if (op_mode_solid_random == 3) {
                op_mode_solid_b = 255;
            }
            op_mode_solid_r_B = 0;
            op_mode_solid_g_B = 0;
            op_mode_solid_b_B = 0;
        }
    }

    if ((unsigned long) (millis() - op_mode_solid_fade_time) > OP_MODE_SOLID_FADE_TIME) {
        op_mode_solid_fade_time = millis();

        if (op_mode_solid_state == 0) {
            if (op_mode_solid_random == 1) {
                if (op_mode_solid_r_B != 255) {
                    op_mode_solid_r_B++;
                }
            } else if (op_mode_solid_random == 2) {
                if (op_mode_solid_g_B != 255) {
                    op_mode_solid_g_B++;
                }
            } else if (op_mode_solid_random == 3) {
                if (op_mode_solid_b_B != 255) {
                    op_mode_solid_b_B++;
                }
            }

            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                strip.setPixelColor(i, strip.Color(op_mode_solid_r_B, op_mode_solid_g_B, op_mode_solid_b_B));
            }
        } else if (op_mode_solid_state == 1) {
            if (op_mode_solid_random == 1) {
                if (op_mode_solid_r_B != 2) {
                    op_mode_solid_r_B--;
                }
            } else if (op_mode_solid_random == 2) {
                if (op_mode_solid_g_B != 2) {
                    op_mode_solid_g_B--;
                }
            } else if (op_mode_solid_random == 3) {
                if (op_mode_solid_b_B != 2) {
                    op_mode_solid_b_B--;
                }
            }

            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                strip.setPixelColor(i, strip.Color(op_mode_solid_r_B, op_mode_solid_g_B, op_mode_solid_b_B));
            }
        }
    }
}

void op_mode_random() {
    if ((unsigned long) (millis() - op_mode_random_change_time) > OP_MODE_RANDOM_TIME) {
        op_mode_random_change_time = millis();
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            op_mode_random_color = random(0, OP_MODE_RANDOM_LENGTH);

            strip.setPixelColor(i, op_mode_random_colors[op_mode_random_color]);
        }
    }
}