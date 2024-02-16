#include <bits/stdc++.h>
#include <M5Atom.h>
#include <Wire.h>
#include <FastLED.h>

const int brightness  =  64;

#define NUM_LEDS 25
#define NUM_MODE 3
#define DATA_PIN 27

// 25個のLEDの色情報を格納する
CRGB leds[NUM_LEDS];
 
const int WAIT = 400;
const int PALSE = 30;

const int ROLE_TIME = 15;

std::mt19937 mt;

std::uniform_int_distribution<int> dice_1d6(1, 6);
std::uniform_int_distribution<int> dice_1d10(0, 9);
std::uniform_int_distribution<int> dice_1d2(0, 1);

std::uniform_int_distribution<int> dice_mode[NUM_MODE] = { dice_1d6, dice_1d10, dice_1d2 };
uint color_mode[NUM_MODE] = { /*緑*/ 0x00ff00, /*赤*/ 0xff0000, /*青*/ 0x0000ff };

int mode_idx = 0;
int roll_idx = 1;

const uint dice_rolls[10][NUM_LEDS] = {
    // 0
    {
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    // 1
    {
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0
    },
    // 2
    {
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    },
    // 3
    {
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    // 4
    {
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0
    },
    // 5
    {
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    // 6
    {
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    // 7
    {
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0
    },
    // 8
    {
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
    },
    // 9
    {
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    }
};

// 振ったか判定する
bool is_shaked() {
    float border = 5.0f;
    
    float acc_x, acc_y, acc_z;
    M5.IMU.getAccelData(&acc_x, &acc_y, &acc_z);
    
    return acc_x * acc_x + acc_y * acc_y + acc_z * acc_z >= border * border;
}

// モードと出目に合った LED の表示を設定する
void set_leds(int mode_idx, int roll_idx) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = dice_rolls[roll_idx][i] * color_mode[mode_idx];
    }
}

void setup() {
    // M5Atom を設定する
    M5.begin(
        /*SerialEnable*/ true, 
        /*I2CEnable*/ false, 
        /*DisplayEnable*/ true
    );

    // FastLED を設定する
    Wire.begin();
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);

    M5.IMU.Init();

    // 温度から乱数のシード値を得る
    float temp;
    M5.IMU.getTempData(&temp);
    int seed = (int)(temp * 100);
    mt.seed(seed);

    // LED を 1 に初期化する
    set_leds(mode_idx, roll_idx);
    FastLED.show();

    delay(WAIT);
}

void loop() {
    M5.update();

    // ボタン押下時、モードを変更する
    if (M5.Btn.wasPressed()) {
        mode_idx = (mode_idx + 1) % NUM_MODE;
        set_leds(mode_idx, roll_idx);
        FastLED.show();
    }

    // 振った時、サイコロを振る
    if (is_shaked()) {
        for (int _ = 0; _ < ROLE_TIME; _++) {
            auto dice = dice_mode[mode_idx];
            roll_idx = dice(mt);
            
            set_leds(mode_idx, roll_idx);
            
            FastLED.show();
            delay(PALSE);
        }
    }
}
