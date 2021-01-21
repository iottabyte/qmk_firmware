/* Copyright 2020 Jay Greco
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Base Layout:
 * ,-----------------------.
 * |     |  F1 |  << |  >> | 
 * |------+------+------+--|
 * |     |  F2 |  F5 | F12 | 
 * |------+------+------+--|
 * |  7  |  8  |  9  |  0  | 
 * |------+------+------+--|
 * |  4  |  5  |  6  |     |
 * |------+------+------+--|
 * |  1  |  2  |  3  |     |
 * `-----------------------`
 */

#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "remote_kb.h"
#include "bitc_led.h"


#define _BASE     0
#define _MISC     1
#define _PHSP     2
#define _MACR     3

enum custom_keycodes {
  PROG = SAFE_RANGE,
};

enum td_keycodes {
    TD_ENTER_LAYER
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // Base layer (numpad)
  [_BASE] = LAYOUT(
           KC_F1,    KC_MEDIA_PREV_TRACK, KC_MEDIA_NEXT_TRACK, \
  KC_NO, KC_F2,  KC_F5,        KC_F12, \
  KC_KP_7, KC_KP_8,  KC_KP_9,        KC_KP_0, \
  KC_KP_4, KC_KP_5,  KC_KP_6,        KC_NO, \
  KC_KP_1,   KC_KP_2,  KC_KP_3,      KC_NO \
  ),
  // Function layer (misc navigation and media controls)
  [_MISC] = LAYOUT(
           KC_NO, KC_MPLY, KC_MUTE,
    KC_NO, KC_NO, RGB_MOD, RGB_MOD,
    KC_HOME, KC_UP, KC_PGUP, RGB_HUI,
    KC_LEFT, KC_NO, KC_RGHT, RGB_SAI,
    KC_END,  KC_DOWN, KC_PGDN, RGB_VAI
  ),
  // Photoshop layer (photoshop shortcuts)
  [_PHSP] = LAYOUT(
            MO(1), KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO,
    KC_HOME, KC_UP, KC_PGUP, RGB_HUI,
    KC_L, KC_SPC, KC_I, KC_NO,
    KC_E,  KC_N, KC_B, KC_NO
  )

  // Macro Layer (random macros)
  [_MACR] = LAYOUT(
            KC_
  )
};

// Initialization function 2: Happens midway through the firmware’s startup process.
// Hardware is initialized, but features may not be yet.
void matrix_init_user(void) {
  matrix_init_remote_kb();
  register_code(KC_NLCK);
}

void matrix_scan_user(void) {
  matrix_scan_remote_kb();
}

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

    default:
    break;
  }
  return true;
}

// Encoder layer select
uint8_t selected_layer = 0;
void encoder_update_user(uint8_t index, bool clockwise) {
  switch (index) {
    case 0:
      if (!clockwise && selected_layer  < 10) {
        selected_layer ++;
      } else if (clockwise && selected_layer  > 0) {
        selected_layer --;
      }
      layer_clear();
      layer_on(selected_layer);
 
  }  
}

// Underglow change based on layer
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case _BASE:
        rgblight_setrgb (0x00,  0x00, 0xFF);
        break;
    case _MISC:
        rgblight_setrgb (0xFF,  0x00, 0x00);
        break;
    case _PHSP:
        rgblight_setrgb (0x00,  0xFF, 0x00);
        break;
    case _MACR:
        rgblight_setrgb (0x7A,  0x00, 0xFF);
        break;
    default: //  for any other layers, or the default layer
        rgblight_setrgb (0x00,  0xFF, 0xFF);
        break;
    }
  return state;
}


void led_set_kb(uint8_t usb_led) {
  if (usb_led & (1<<USB_LED_CAPS_LOCK))
    set_bitc_LED(LED_DIM);
  else
    set_bitc_LED(LED_OFF);
}

void suspend_power_down_user(void) {
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_user(void) {
    rgb_matrix_set_suspend_state(false);
}
