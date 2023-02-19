#include <Arduino.h>

#define NO_GLOBAL_INSTANCES
#include <PS4Controller.h>

PS4Controller ps4;

// Timer
unsigned long int color_sampling_timer;

// Sampling time in ms
int color_time = 50;

// Pins
const int ledR = 18;
const int ledG = 19;
const int ledB = 21;

// Channels
const int chanR = 0x0;
const int chanG = 0x1;
const int chanB = 0x2;

// PWM
const int freq = 5000;
const int res = 8;

int R = 0;
int G = 0;
int B = 250;
int serialCounter = 0;
int colorDelta = 5;

bool color_changed = false;

int abs(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

void setColorImmediate(int r, int g, int b) {
    R = r;
    G = g;
    B = b;
    ps4.setLed(abs(R), abs(G), abs(B));
    ps4.sendToController();
    ledcWrite(chanR, abs(R));
    ledcWrite(chanG, abs(G));
    ledcWrite(chanB, abs(B));
}

void setRGB() {
    if (ps4.Left() || ps4.Right() || ps4.L2() || ps4.R2() || ps4.Square() || ps4.Circle() || ps4.Options()) {
        if (ps4.Left()) {
        R -= colorDelta;

            if (R < -255) {
                R += 510;
            }
        }

        if (ps4.Right()) {
            R += colorDelta;

            if (R > 255) {
                R -= 510;
            }
        }

        if (ps4.L2()) {
            G -= colorDelta;

            if (G < -255) {
                G += 510;
            }
        }

        if (ps4.R2()) {
            G += colorDelta;

            if (G > 255) {
                G -= 510;
            }
        }

        if (ps4.Square()) {
            B -= colorDelta;

            if (B < -255) {
                B += 510;
            }
        }

        if (ps4.Circle()) {
            B += colorDelta;

            if (B > 255) {
                B -= 510;
            }
        }

        if (ps4.Options()) {
            setColorImmediate(0, 0, 255);
        }

        ps4.setLed(abs(R), abs(G), abs(B));
        color_changed = true;
    }
}

void setup() {
    ps4.begin("00:1b:10:00:2a:ee");
    ledcSetup(chanR, freq, res);
    ledcSetup(chanG, freq, res);
    ledcSetup(chanB, freq, res);
    ledcAttachPin(ledR, chanR);
    ledcAttachPin(ledG, chanG);
    ledcAttachPin(ledB, chanB);
    setColorImmediate(0, 0, 255);
    color_sampling_timer = millis();
}

void loop() {
    if (millis() - color_sampling_timer > color_time) {
        setRGB();
        if (color_changed) {
            ps4.sendToController();
            ledcWrite(chanR, abs(R));
            ledcWrite(chanG, abs(G));
            ledcWrite(chanB, abs(B));
            color_changed = false;
        }

        color_sampling_timer = millis();
    }
}
