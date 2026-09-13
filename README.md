# arch-setup-tui
A clean, minimalist C99 &amp; Whiptail TUI for building custom Arch Linux desktop environments.

# Arch Rice Builder

A lightweight, clean, and minimalist TUI (Text User Interface) built in C99 to assemble custom Arch Linux environments from scratch.

![Preview](https://raw.githubusercontent.com/seu-usuario/seu-repositorio/main/preview.png)

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
