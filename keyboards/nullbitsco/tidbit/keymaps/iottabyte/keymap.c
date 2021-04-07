/* Copyright 2020 Jay Greco
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/* Base Layout:
 * ,-----------------------.
 * |     |  F1 |  << |  >> | 
 * |------+------+------+--|
 * |     |  F2 |  F5 | F12 | 
 * |------+------+------+--|
 * |  7  |  8  |  9  | DEL | 
 * |------+------+------+--|
 * |  4  |  5  |  6  |  /  |
 * |------+------+------+--|
 * |  1  |  2  |  3  |  0  |    
 * `-----------------------`
 */

#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "remote_kb.h"
#include "bitc_led.h"
#include "oled_hid.h"


#define _BASE     0
#define _MISC     1
#define _PHSP     2
#define _STUP     3

enum custom_keycodes {
  PROG = SAFE_RANGE,
  SEL,
  DESEL,
  LIQ,
  TRFM,
  N_LYR,
  UNDO,
  NXTSC,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // Base layer (numpad & useful keys)
  [_BASE] = LAYOUT(
           KC_F1,    KC_MPRV, KC_MNXT, \
  MO(2), KC_F2,  KC_F5,      KC_F12, \
  KC_KP_7, KC_KP_8,  KC_KP_9,  KC_DEL, \
  KC_KP_4, KC_KP_5,  KC_KP_6,  KC_SLSH, \
  KC_KP_1,   KC_KP_2,  KC_KP_3, KC_KP_0 \
  ),
  
  // Function layer (misc navigation and media controls)
  [_MISC] = LAYOUT(
           KC_WFAV, KC_MPLY, KC_MUTE, \
    KC_NO, KC_BSPC, RGB_MOD, KC_PSCR, \
    KC_HOME, KC_UP, KC_PGUP, RGB_TOG, \
    KC_LEFT, KC_LSFT, KC_RGHT, KC_ENT, \
    KC_END,  KC_DOWN, KC_PGDN, RGB_M_P \
  ),

  // Photoshop layer (photoshop shortcuts)
  [_PHSP] = LAYOUT(
            MO(1), N_LYR, LIQ, \
    KC_ENT, KC_CUT, KC_COPY, KC_PSTE, \
    KC_LBRC, KC_RBRC, TRFM, SEL, \
    KC_L, KC_SPC, KC_I, DESEL, \
    KC_E,  KC_N, KC_B, UNDO \
  ),

  // Setup layer (configuration keys)
  [_STUP] = LAYOUT(
            PROG, KC_NO, NXTSC, \
  KC_NO, KC_NO, KC_NO, KC_TRNS, \
  KC_MYCM, KC_NO, KC_NO, KC_TRNS, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_PWR, KC_EJCT, KC_NO, KC_SLEP  \
  ),

};

void matrix_init_user(void) {
  matrix_init_remote_kb();
}

void matrix_scan_user(void) {
  matrix_scan_remote_kb();
}

void oled_task_user(void) {

}

// custom keys and macros
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  process_record_remote_kb(keycode, record);

  switch(keycode) {
    case PROG:
      if (record->event.pressed) {
        set_bitc_LED(LED_DIM);
        rgblight_disable_noeeprom();
        bootloader_jump(); //jump to bootloader
      }
    break;

    case UNDO:
      if (record->event.pressed) {
          SEND_STRING(SS_LCTL("z"));
      } 
      break;

    case SEL: // select all
      if (record->event.pressed) {
          SEND_STRING(SS_LCTL("a"));
      }
      break;

    case DESEL: // deselect selection
      if (record->event.pressed) {
          SEND_STRING(SS_LCTL("d"));
      }
      break;

    case LIQ: // open liquify interface
      if (record->event.pressed) {
          SEND_STRING(SS_DOWN(X_LSHIFT)SS_DOWN(X_LCTRL)SS_TAP(X_X)SS_UP(X_LCTRL)SS_UP(X_LSHIFT));
      }
      break;

    case TRFM: // transform object
      if (record->event.pressed) {
          SEND_STRING(SS_LCTL("t"));
      }
      break;

    case N_LYR: // make new layer without dialogue box
      if (record->event.pressed) {
          SEND_STRING(SS_DOWN(X_LCTRL)SS_DOWN(X_LSHIFT)SS_DOWN(X_LALT)SS_TAP(X_N)SS_UP(X_LALT)SS_UP(X_LSHIFT)SS_UP(X_LCTRL));
      }
      break;

    case NXTSC: // advance to next OLED screen
        if (record->event.pressed) {
            update_oled();
        }
       break;
  }
  return true;
}

// encoder layer select
uint8_t selected_layer = 0;
void encoder_update_user(uint8_t index, bool clockwise) {
  switch (index) {
    case 0:
      if (!clockwise && selected_layer  < 4) {
        selected_layer ++;
      } else if (clockwise && selected_layer  > 0) {
        selected_layer --;
      }
      layer_clear();
      layer_on(selected_layer);
 
  }  
}

// underglow change based on layer (WS2812B)
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _MISC: // green
        rgblight_setrgb (0x72,  0xCB, 0x42);
        break;
    case _PHSP: // soft blue
        rgblight_setrgb (0x9E,  0xB5, 0xE0);
        break;
    case _STUP: // goldenrod
        rgblight_setrgb (0xFB,  0xD4, 0x26);
        break;
    default: //  for any other layers, or the default layer (pink)
        rgblight_setrgb (0xEC,  0x55, 0x93);
        break;
    }
  return state;
}

// LED control on micro controller
void led_set_kb(uint8_t usb_led) {
  if (usb_led & (1<<USB_LED_CAPS_LOCK))
    set_bitc_LED(LED_DIM);
  else
    set_bitc_LED(LED_OFF);
}


