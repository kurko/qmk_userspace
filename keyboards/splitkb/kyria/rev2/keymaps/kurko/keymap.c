/* Copyright 2022 Alex Oliveira <@kurko>
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
#include QMK_KEYBOARD_H

enum layers {
    _NORMAL = 0,
    _SYM_NUM,
    _FUNCTION,
    _ADJUST,
};


/**
 * CUSTOM KEYCODES
 *
 * These are the keycodes that are used for the tap-hold functionality. They
 * must be in the range of SAFE_RANGE to END_CUSTOM_KEYCODES. For example, we
 * want to tap Q for Q, and hold Q for 1 because this keyboard doesn't have the
 * numbers row.
 */
enum custom_keycodes {
  Q_1 = SAFE_RANGE,
  W_2,
  E_3,
  R_4,
  T_5,
  Y_6,
  U_7,
  I_8,
  O_9,
  P_0,
  //...
  END_CUSTOM_KEYCODES, // Always keep this one at the end!
};

// Replace 10 with the number of custom keycodes we have
#define CUSTOM_KEYCODES_LENGTH (END_CUSTOM_KEYCODES - SAFE_RANGE)

/**
 * Custom keycodes
 *
 * This keeps a mapping of the custom keycodes to their respective 'letter' and
 * 'number' keycodes. The 'letter' keycode is sent when the key is tapped,
 * whereas the 'number' keycode is sent when the key is held for longer than
 * TAPPING_TERM.
 */
uint16_t custom_keys[CUSTOM_KEYCODES_LENGTH][2] = {
    {KC_Q, KC_1},
    {KC_W, KC_2},
    {KC_E, KC_3},
    {KC_R, KC_4},
    {KC_T, KC_5},
    {KC_Y, KC_6},
    {KC_U, KC_7},
    {KC_I, KC_8},
    {KC_O, KC_9},
    {KC_P, KC_0},
};

// Basic state management for the tap-hold functionality
bool is_custom_key_pressed;
// Keeps track of which keys have been tapped so the timer doesn't try to
// trigger the tap-hold functionality for keys that have already been tapped.
bool custom_keys_tapped[CUSTOM_KEYCODES_LENGTH];
bool custom_keys_timer_initialized = false;
uint16_t custom_keys_timer[CUSTOM_KEYCODES_LENGTH];
// The index of the key that is currently being held down
uint16_t custom_key_down;

// Aliases for readability
#define NORMAL   DF(_NORMAL)
#define SYM_NUM  DF(_SYM_NUM)
#define FKEYS    MO(_FUNCTION)
#define ADJUST   MO(_ADJUST)

#define CTL_ESC  MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
/*
 * This shortcut is using for toggling input sources from e.g US to US
 * International. It's using HYPR because it's very unlikely that that would be
 * set in a new system, so there are less chances that this would cause trouble
 * in new computers.
 */
#define CHANGE_SOURCE HYPR(KC_S)
/*
 * Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
 * The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
 * produces the key `tap` when tapped (i.e. pressed and released).
 */
#define ALT_ENT  MT(MOD_LALT, KC_ENT)

void keyboard_post_init_user(void) {
    /*
     * Initialize the custom keycodes timer array. This is used to keep track of
     * how long a key has been held down for.
     */
    for (int i = 0; i < CUSTOM_KEYCODES_LENGTH; i++) {
        custom_keys_tapped[i] = true;
        custom_keys_timer[i] = 0;
    }
    custom_keys_timer_initialized = true;

#ifdef RGBLIGHT_ENABLE
    rgblight_enable_noeeprom(); // Enables RGB, without saving settings
    /*
     * Starts out with a dark green color.
     */
    rgblight_sethsv_noeeprom(85, 255, 60);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
#endif
}


/**
 * Tap-hold declarations
 *
 *
 */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_ESC:
            return 130;
        default:
            return TAPPING_TERM;
    }
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    bool is_custom_keycode = keycode >= Q_1 && keycode < END_CUSTOM_KEYCODES;
    /**
     * TAP-HOLD: CANCEL HOLD ON ANOTHER KEY ON FAST SEQUENCE
     *
     * If we're holding a tap-hold custom key and another key is pressed, we
     * should cancel the hold on the custom key.
     *
     * For example, Q tapped is Q and Q held is 1. If we are holding Q and
     * immediately type U, we cancel the hold on Q, send Q instead of 1, and
     * send U.
     *
     * Note that this wouldn't work with modifiers like CTRL (because we want to
     * keep the modifier held). That's why we're not using modified for this
     * custom tap-held code. For those we use `MT(MOD_LCTL, KC_ESC)`.
     */
    if (
            is_custom_key_pressed &&
            custom_key_down >= 0 &&
            record->event.pressed
       ) {
        tap_code(custom_keys[custom_key_down][0]);
        custom_keys_tapped[custom_key_down] = true;
        custom_keys_timer[custom_key_down] = 0;
        custom_key_down = 0;
        is_custom_key_pressed = false;
    }

    /**
     * TAP-HOLD: HANDLE CUSTOM KEYCODES
     *
     * Handle the keycode if it's within the custom keycodes range. It works in
     * tandem with matrix_scan_user(), which will send the 'letter' keycode when
     * the timer is greater than TAPPING_TERM.
     */
    if (is_custom_keycode) {
        int keyIndex = keycode - Q_1;
        /**
         * When pressed, just start a timer.
         */
        if (record->event.pressed) {
            is_custom_key_pressed = true;
            custom_key_down = keyIndex;
            custom_keys_timer[keyIndex] = timer_read();
            custom_keys_tapped[keyIndex] = false;

        /**
         * When released and not yet processed by matrix_scan_user() (which sets
         * custom_keys_tapped), it means that the key was tapped before
         * TAPPING_TERM, so we send the 'letter' keycode.
         */
        } else if(!custom_keys_tapped[keyIndex]) {
            tap_code(custom_keys[keyIndex][0]);
            is_custom_key_pressed = false;
            custom_keys_timer[keyIndex] = 0;
            custom_keys_tapped[keyIndex] = true;
        }
        return false; // Skip all further processing of this key
    }

    return true; // Process all other keycodes normally
}

/**
 * The matrix_scan_user function in QMK is called at a regular interval (every
 * matrix scan), allowing for custom code to be executed repeatedly while the
 * keyboard is on. In the context of your custom functionality, it's used to
 * check if the custom keys have been held down for longer than the tapping
 * term. If they have, it triggers the sending of the 'number' keycode, ensuring
 * that the hold action is recognized and executed even before the key is
 * released.
 */
void matrix_scan_user(void) {

    /**
     * Don't do anything unless we have initialized the system and some custom
     * key is pressed.
     */
    if (custom_keys_timer_initialized && is_custom_key_pressed) {
        for (uint16_t keyIndex = 0; keyIndex < CUSTOM_KEYCODES_LENGTH; keyIndex++) {
            if (
                    timer_elapsed(custom_keys_timer[keyIndex]) > TAPPING_TERM &&
                    !custom_keys_tapped[keyIndex]
               ) {

                tap_code_delay(custom_keys[keyIndex][1], 250);
                custom_keys_timer[keyIndex] = 0;
                custom_keys_tapped[keyIndex] = true;
                is_custom_key_pressed = false;
            }
        }
    }
}

/*
 * Sets the RGB colors for each layer.
 */
layer_state_t layer_state_set_user(layer_state_t state) {
#ifdef RGBLIGHT_ENABLE
    switch (get_highest_layer(state)) {
      case _NORMAL:
          rgblight_sethsv_noeeprom(85, 255, 60);
          break;
      case _SYM_NUM:
          //rgb_matrix_set_color_all(0,0,0);
          rgblight_sethsv_noeeprom(85, 255, 100);
          break;
      case _FUNCTION:
          rgblight_sethsv_noeeprom(148, 255, 100);
          break;
      case _ADJUST:
          rgblight_sethsv_noeeprom(148, 255, 130);
          break;
      default:
          rgblight_sethsv_noeeprom(0, 255, 100);
          break;
    }
#endif
    return state;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Normal Layer: this is the layer used for writing. The keyboard always gets
 * back to this layer (usually automatically).
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |      |      |  |  L1  |      |   N  |   M  | ,  < | . >  | /  ? |   \    |
 * `----------------------+------+------+------+--  --+------|  |------+--  --+------+------+------+----------------------'
 *                        |      | LAlt | LGUI | Space|      |  | Temp | Enter| RGUI | RAlt | Menu |
 *                        |      |      |      |      |      |  |  L1  |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NORMAL] = LAYOUT(
        KC_TAB,  Q_1,  W_2,  E_3,  R_4,  T_5,                                      Y_6,  U_7,  I_8,     O_9,    P_0,    KC_BSPC,
        CTL_ESC, KC_A, KC_S, KC_D, KC_F, KC_G,                                     KC_H, KC_J, KC_K,    KC_L,   KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,       KC_NO,  KC_NO, TO(1), KC_NO,  KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_BSLS,
                    CHANGE_SOURCE, KC_LALT, KC_LGUI, KC_SPC, KC_NO, TT(1), KC_ENT, KC_RGUI, KC_RALT, KC_NO
    ),

/*
 * Symbols and Numbers Layer: this is used usually in temporary mode, as in,
 * thumb is clicking TT(1) while index or middle fingers accessing some other
 * key.
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  ` ~   |  1 ! |  2 @ |  3 # |  4 $ |  5 % |                              |  6 ^ |  7 & |  8 * |  9 ( |  0 ) |  Bksp  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|      |      |      |      |      |                              |  - _ |  = + |UArrow|  [ { |  ] } |  -  _  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |      |      |      |      |      |      |      |  |  L2  |      |      |LArrow|DArrow|RArrow|      |        |
 * `----------------------+------+------+------+--  --+------|  |------+------+------+------+------+----------------------'
 *                        |      | LAlt | LGUI | Space|  L1  |  | Temp | Enter| RGUI | RAlt |      |
 *                        |      |      |      |      |      |  |  L2  |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM_NUM] = LAYOUT(
        KC_GRV,  KC_1,  KC_2,  KC_3,  KC_4,  KC_5,                                       KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        CTL_ESC, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                                      KC_MINS, KC_EQL,  KC_UP,   KC_LBRC, KC_RBRC, CTL_MINS,
        KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,     KC_NO,  KC_NO,  TO(2),  KC_NO,   KC_NO,   KC_LEFT, KC_DOWN, KC_RGHT, KC_NO,   KC_NO,
                               KC_NO, KC_LALT, KC_LGUI, KC_SPC, TO(0),  TT(2),  KC_ENT,  KC_RGUI, KC_RALT, KC_NO
    ),

/*
 * Functions Layer: this focused on function keys and less used keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F1  |  F2  |  F3  |  F4  |  F5  |                              |  F6  |  F7  |  F8  |  F9  |  F10 |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|      |      |      |      |      |                              |      |      |      |      |  F11 |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |      |      |      |      |      |      |      |  |  L3  |      |      |      |      |      |  F12 |        |
 * `----------------------+------+------+------+--  --+------|  |------+------+------+------+------+----------------------'
 *                        |      | LAlt | LGUI | Space|  L0  |  | Temp | Enter| RGUI | RAlt |      |
 *                        |      |      |      |      |      |  |  L3  |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT(
        KC_NO,   KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                                      KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_NO,
        CTL_ESC, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                                      KC_NO, KC_NO, KC_NO, KC_NO, KC_F11, KC_NO,
        KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,      KC_NO,  KC_NO, TO(3), KC_NO,    KC_NO, KC_NO, KC_NO, KC_NO, KC_F12, KC_NO,
                               KC_NO, KC_LALT, KC_LGUI,  KC_SPC, TO(0), TT(3), KC_ENT,   KC_LGUI, KC_RALT, KC_NO
    ),

    [_ADJUST] = LAYOUT(
      _______, _______, _______, NORMAL , _______, _______,                                    _______, _______, _______, _______,  _______, _______,
      _______, _______, _______, SYM_NUM, _______, _______,                                    RGB_TOG, RGB_SAI, RGB_HUI, RGB_VAI,  RGB_MOD, _______,
      _______, _______, _______, _______, _______, _______,_______, _______, TO(0),   _______, _______, RGB_SAD, RGB_HUD, RGB_VAD, RGB_RMOD, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______
    ),

 /*
  * Layer template
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
  /* [_LAYERINDEX] = LAYOUT(
   *   _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
   *   _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
   *   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   *                              _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
   * ),
   */
};

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (get_highest_layer(layer_state) == 0) {
            // L/R arrows
            if (clockwise) {
                tap_code(KC_DOWN);
            } else {
                tap_code(KC_UP);
            }
        } else if (get_highest_layer(layer_state) == 1) {
            // U/D arrows
            if (clockwise) {
                tap_code(KC_RIGHT);
            } else {
                tap_code(KC_LEFT);
            }
        } else if (get_highest_layer(layer_state) == 2) {
            // Page up/Page down
            if (clockwise) {
                tap_code(KC_PGDN);
            } else {
                tap_code(KC_PGUP);
            }
        }
    } else if (index == 1) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return false;
}
#endif
