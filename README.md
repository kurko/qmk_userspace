# kurko's QMK Userspace

Welcome to my QMK userspace repository! It contains my custom configurations,
keymaps, and code enhancements for QMK-powered keyboards.

## Keyboards Supported

- **Sweep**: A minimalist split keyboard with 34 keys.

## Features

- **Tap-Hold Functionality**: Keys perform different actions when tapped or held, allowing for more commands without extra keys.
- **Layered Layouts**: Multiple layers for symbols, navigation, media controls, and more.
- **Custom Macros**: Frequently used commands and shortcuts mapped to single keys.
- **Combos**: Special actions triggered by pressing multiple keys simultaneously.
- **One-Shot Modifiers**: Modifiers that apply to the next key pressed without needing to hold them down.

## Installation

To use these configurations with your QMK setup:

1. Run the normal `qmk setup` procedure if you haven't already done so -- see
   [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository, or clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`

To add new keyboards, add a new keymap for your board using `qmk new-keymap`

## License

This project is licensed under the terms of the
[GNU General Public License v2.0](LICENSE).

## Acknowledgments

- **QMK Community**: For the extensive documentation and support.
- **Inspiration**: Layout ideas inspired by [Ben Vallack's keyboard layout](https://www.youtube.com/watch?v=8wZ8FRwOzhU).
