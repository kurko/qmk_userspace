# Kyria Rev2 Custom Keymap

![Kyria split keyboard](https://i.ibb.co/RQZx2dY/default-kyria2.jpg)

Custom keymap for the [Kyria split keyboard](https://blog.splitkb.com/blog/introducing-the-kyria) (rev2). This configuration is designed for the Elite-C controller and uses US/US International keyboard layouts.

## Overview

The Kyria is a 46-key ergonomic split keyboard with thumb clusters, rotary encoder support, and RGB underglow. This keymap implements a custom tap-hold system that eliminates the need for a dedicated number row by making the top QWERTY row dual-purpose: tap for letters, hold for numbers.

## Current Status

This configuration is functional but not as polished as the Aurora Sweep configuration. The Aurora Sweep has received more recent refinements, particularly around timing consistency for custom keycodes. Those improvements are candidates for being ported back to this Kyria configuration.

The Kyria configuration was created to address the limitations of the 34-key Aurora Sweep for management work, where more frequent mouse usage and single-hand keyboard positioning make the additional keys valuable.

## Layer Structure

The keymap uses four layers:

1. **Normal Layer (_NORMAL)**: Primary layer for typing
2. **Symbols/Numbers Layer (_SYM_NUM)**: Numbers, symbols, and navigation
3. **Functions Layer (_FUNCTION)**: Function keys (F1-F12)
4. **Adjust Layer (_ADJUST)**: RGB controls and keyboard settings

## Custom Tap-Hold System

### How It Works

The top row (Q-W-E-R-T-Y-U-I-O-P) implements custom tap-hold functionality:
- **Tap**: Outputs the letter (Q, W, E, etc.)
- **Hold** (>180ms): Outputs the number (1, 2, 3, etc.)

This is implemented using custom keycodes (Q_1, W_2, E_3, etc.) rather than QMK's built-in mod-tap functionality to allow for more precise control over the behavior.

### Intelligent Key Interruption

When typing quickly, the system intelligently handles interruptions to prevent unintended number outputs:

**Example**: If you're holding Q (which would normally output 1 after 180ms) and you quickly press U:
- The system cancels the hold on Q
- Outputs Q immediately
- Processes U normally
- Result: "qu" not "1u"

This is handled in `process_record_user()` in keymap.c:143-205.

### Implementation Details

The tap-hold system uses:
- Custom keycode range: Q_1 through P_0 (SAFE_RANGE to END_CUSTOM_KEYCODES)
- State tracking arrays to monitor each key's press status
- Timer-based evaluation in `matrix_scan_user()` for hold detection
- A 250ms delay on hold output to ensure proper registration

## Configuration

### Timing

- **Default tapping term**: 180ms (config.h:35)
- **Ctrl/Esc tapping term**: 130ms (shorter for better responsiveness)
- **Hold output delay**: 250ms (keymap.c:229)

The tapping term of 180ms was chosen as a balance between fast typing and reliable hold detection.

### RGB Lighting

Visual feedback through RGB underglow indicates the active layer:

- **Normal Layer**: Dark green (HSV: 85, 255, 60)
- **Symbols/Numbers Layer**: Brighter green (HSV: 85, 255, 100)
- **Functions Layer**: Cyan (HSV: 148, 255, 100)
- **Adjust Layer**: Bright cyan (HSV: 148, 255, 130)

RGB settings (config.h:24-30):
- Brightness limit: 150 (to prevent excessive power draw)
- Step sizes: 8 for hue, saturation, and value adjustments

### Rotary Encoder

The keyboard supports dual rotary encoders with layer-specific behavior:

**Left Encoder (index 0)**:
- Normal Layer: Up/Down arrows
- Symbols Layer: Left/Right arrows
- Functions Layer: Page Up/Page Down

**Right Encoder (index 1)**:
- All layers: Volume Up/Volume Down

Encoder resolution is set to 2 for both sides (config.h:20-21).

### Special Features

**Input Source Switcher**: The keyboard includes a custom input source toggle using `HYPR(KC_S)` (Ctrl+Shift+Alt+Cmd+S). This is mapped to the left thumb cluster and is used for switching between keyboard layouts (e.g., US to US International). The Hyper modifier was chosen because it's unlikely to conflict with existing system shortcuts.

## Known Limitations & Future Improvements

### Potential Aurora Sweep Improvements to Port

The Aurora Sweep configuration has received several timing-related improvements that should be considered for the Kyria:

1. **Timing Consistency** (commit bffac9b): Custom keycodes in Aurora now use a 35ms delay between key down/up events to match standard key timing and prevent out-of-order outputs during fast typing. The Kyria currently uses a 250ms delay on hold output, which may be excessive.

2. **Space Key Logic Refinement** (commit 9acbcd7): Improvements to prevent out-of-order key events with space.

3. **General Timing Adjustments** (commits 5060083, ef7a12a): Various refinements to tapping terms and delay removal.

### What Wasn't Kept

The initial implementation note mentions that `IGNORE_MOD_TAP_INTERRUPT` was removed because it became default behavior in modern QMK firmware. This is documented in config.h:32-34.

## Hardware

- **Controller**: Elite-C
- **Features enabled**: RGB underglow, rotary encoders
- **Features disabled**: OLED displays (to conserve resources on Elite-C)

## Build

This keymap is part of the QMK userspace repository. Build target: `splitkb/kyria/rev2`

## Files

- `keymap.c`: Main keymap implementation with custom tap-hold logic
- `config.h`: Hardware and timing configuration
- `rules.mk`: Feature flags and build options
