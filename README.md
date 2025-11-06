# Alex's QMK userspace

Welcome to my QMK userspace repository! It contains my custom configurations,
keymaps, and code enhancements for QMK-powered keyboards.

## Keyboards supported

- **Aurora Sweep**: A minimalist split keyboard with 34 keys.
- **Kyria**: A 46 key split keyboard with thumb clusters and encoders (guitar
    knobs).

## Features

- **Tap-Hold Functionality**: Keys perform different actions when tapped or held, allowing for more commands without extra keys.
- **Layered Layouts**: Multiple layers for symbols, navigation, media controls, and more.
- **Custom Macros**: Frequently used commands and shortcuts mapped to single keys.
- **Combos**: Special actions triggered by pressing multiple keys simultaneously.
- **One-Shot Modifiers**: Modifiers that apply to the next key pressed without needing to hold them down.

These are not implemented evenly. Some keyboards have more features than
others, based on their frequency of usage and how challenging they are in
relation to how many keys they have.

## Installation

To use these configurations with your QMK setup:

1. Run the normal `qmk setup` procedure if you haven't already done so -- see
   [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository, or clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`

To add new keyboards, add a new keymap for your board using `qmk new-keymap`

## Flashing your keyboard

Once flashing, it will halt and wait for you to double-click the keyboard's
flash button, which puts the keyboard in bootloader mode.

To push the Kyria, run:

    qmk flash -kb splitkb/kyria/rev2 -km kurko

To push the Aurora Sweep, run:

    qmk flash -kb splitkb/aurora/sweep/rev1 -km kurko -e CONVERT_TO=liatris

## License

This project is licensed under the terms of the
[GNU General Public License v2.0](LICENSE).

## Acknowledgments

- **QMK Community**: For the extensive documentation and support.
- **Inspiration**: Layout ideas inspired by [Ben Vallack's keyboard layout](https://www.youtube.com/watch?v=8wZ8FRwOzhU).
