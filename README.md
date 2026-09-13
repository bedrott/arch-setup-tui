# Arch Setup Tui

A lightweight, clean, and minimalist TUI (Text User Interface) built in C99 to assemble custom Arch Linux environments from scratch.

<img width="884" height="567" alt="image" src="https://github.com/user-attachments/assets/f360bba0-4132-4dfc-ab8e-60d16601dc9c" />

## Features

- **Minimalist Aesthetic:** High-contrast monochrome theme with smooth selection highlights.
- **Pure C99 & Whiptail:** Fast execution with zero heavy dependencies or external script overhead.
- **Custom Modular Selection:** Pick only what you need across Desktop Environments, Window Managers, File Managers, Terminals, Widgets, and CLI Tools.
- **Non-Destructive Navigation:** Full ESC / Cancel support that safely returns to the main menu without modifying selection state.

## Categories

- **Desktop Environments / Window Managers** (Hyprland, Niri, Mango, KDE, GNOME, etc.)
- **Display Managers** (SDDM, GDM, LightDM)
- **File Managers** (Dolphin, Nautilus, Yazi, etc.)
- **Terminals** (Kitty, Alacritty, Ghostty, Foot, etc.)
- **Text Editors** (Neovim, VSCodium, Zed, Micro, etc.)
- **Browsers** (Firefox, Brave, Zen Browser, etc.)
- **Shells** (Zsh, Fish, Bash completions)
- **Widgets & Bars** (Waybar, Eww, AGS, Rofi, Dunst, etc.)
- **CLI Tools** (Fastfetch, Btop, Cava, Eza, Zoxide, etc.)

## Building and Running

### Prerequisites

Ensure `gcc` and `whiptail` (usually provided by `libnewt`) are installed:

```bash
sudo pacman -S gcc libnewt
```
### Compilation

Clone the repository and compile with standard C99:

```bash
git clone [https://github.com/bedrott/arch-setup-tui.git](https://github.com/bedrott/arch-setup-tui.git)
```
```
cd arch-setup-tui
```
```
gcc -std=c99 archsetuptui.c -o archsetuptui
```
## Execution

Run the binary with root privileges to allow pacman execution:
Bash
```
sudo ./archsetuptui
```
