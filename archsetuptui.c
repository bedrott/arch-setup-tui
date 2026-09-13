#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define PACMAN_INSTALL "sudo pacman -S --needed --noconfirm "
#define TEMP_FILE      "/tmp/installer_selection.txt"

#define MAX_PACKAGES   256
#define MAX_PKG_LEN    64
#define MAX_ITEMS      32
#define MAX_CMD_LEN    8192

typedef struct {
    const char *tag;
    const char *desc;
} PkgItem;

char selected_packages[MAX_PACKAGES][MAX_PKG_LEN];
int  selected_count = 0;

int is_selected(const char *pkg) {
    int i;
    for (i = 0; i < selected_count; i++) {
        if (strcmp(selected_packages[i], pkg) == 0) {
            return 1;
        }
    }
    return 0;
}

void add_package(const char *pkg) {
    if (is_selected(pkg)) {
        return;
    }
    if (selected_count < MAX_PACKAGES) {
        strncpy(selected_packages[selected_count], pkg, MAX_PKG_LEN - 1);
        selected_packages[selected_count][MAX_PKG_LEN - 1] = '\0';
        selected_count++;
    }
}

void remove_package(const char *pkg) {
    int i, j;
    for (i = 0; i < selected_count; i++) {
        if (strcmp(selected_packages[i], pkg) == 0) {
            for (j = i; j < selected_count - 1; j++) {
                strncpy(selected_packages[j], selected_packages[j + 1], MAX_PKG_LEN);
            }
            selected_count--;
            return;
        }
    }
}

int run_command_status(const char *cmd) {
    int raw = system(cmd);
    if (raw == -1) {
        return -1;
    }
    if (WIFEXITED(raw)) {
        return WEXITSTATUS(raw);
    }
    return -1;
}

int run_checklist(const char *title, const char *text,
                   PkgItem items[], int item_count,
                   char results[][MAX_PKG_LEN], int max_results) {
    char cmd[MAX_CMD_LEN];
    int  len;
    int  i;

    remove(TEMP_FILE);

    len = snprintf(cmd, sizeof(cmd),
                    "whiptail --title \"%s\" --checklist \"%s\" 22 78 15 ",
                    title, text);

    for (i = 0; i < item_count && len < (int)sizeof(cmd); i++) {
        const char *state = is_selected(items[i].tag) ? "ON" : "OFF";
        len += snprintf(cmd + len, sizeof(cmd) - len,
                         "\"%s\" \"%s\" %s ", items[i].tag, items[i].desc, state);
    }
    snprintf(cmd + len, sizeof(cmd) - len, "2> %s", TEMP_FILE);

    int status = run_command_status(cmd);
    if (status != 0) {
        remove(TEMP_FILE);
        return -1;
    }

    FILE *fp = fopen(TEMP_FILE, "r");
    if (fp == NULL) {
        return -1;
    }

    char line[2048] = "";
    if (fgets(line, sizeof(line), fp)) {
    }
    fclose(fp);
    remove(TEMP_FILE);

    int count = 0;
    char *ptr = line;
    while (*ptr != '\0' && count < max_results) {
        char *start = strchr(ptr, '"');
        if (start == NULL) break;
        char *end = strchr(start + 1, '"');
        if (end == NULL) break;

        int token_len = (int)(end - start - 1);
        if (token_len >= MAX_PKG_LEN) {
            token_len = MAX_PKG_LEN - 1;
        }

        strncpy(results[count], start + 1, token_len);
        results[count][token_len] = '\0';
        count++;

        ptr = end + 1;
    }

    return count;
}

int run_menu(const char *title, const char *text,
             PkgItem items[], int item_count, char *result) {
    char cmd[MAX_CMD_LEN];
    int  len;
    int  i;

    remove(TEMP_FILE);

    len = snprintf(cmd, sizeof(cmd),
                    "whiptail --title \"%s\" --menu \"%s\" 22 78 15 ",
                    title, text);

    for (i = 0; i < item_count && len < (int)sizeof(cmd); i++) {
        len += snprintf(cmd + len, sizeof(cmd) - len,
                         "\"%s\" \"%s\" ", items[i].tag, items[i].desc);
    }
    snprintf(cmd + len, sizeof(cmd) - len, "2> %s", TEMP_FILE);

    int status = run_command_status(cmd);
    if (status != 0) {
        remove(TEMP_FILE);
        return 0;
    }

    FILE *fp = fopen(TEMP_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
    if (fgets(result, MAX_PKG_LEN, fp) == NULL) {
        fclose(fp);
        remove(TEMP_FILE);
        return 0;
    }
    fclose(fp);
    remove(TEMP_FILE);

    result[strcspn(result, "\n")] = '\0';
    return 1;
}

int run_yesno(const char *title, const char *text) {
    char cmd[MAX_CMD_LEN + 512];
    snprintf(cmd, sizeof(cmd), "whiptail --title \"%s\" --yesno \"%s\" 15 74", title, text);
    return run_command_status(cmd) == 0;
}

void run_msgbox(const char *title, const char *text) {
    char cmd[MAX_CMD_LEN + 512];
    snprintf(cmd, sizeof(cmd), "whiptail --title \"%s\" --msgbox \"%s\" 14 74", title, text);
    run_command_status(cmd);
}

void sync_packages(PkgItem items[], int item_count, char results[][MAX_PKG_LEN], int result_count) {
    int i, j;
    for (i = 0; i < item_count; i++) {
        int checked = 0;
        for (j = 0; j < result_count; j++) {
            if (strcmp(items[i].tag, results[j]) == 0) {
                checked = 1;
                break;
            }
        }
        if (checked) {
            add_package(items[i].tag);
        } else {
            remove_package(items[i].tag);
        }
    }
}

void pick_packages(const char *title, const char *text, PkgItem items[], int item_count) {
    char results[MAX_ITEMS][MAX_PKG_LEN];
    int  count = run_checklist(title, text, items, item_count, results, MAX_ITEMS);
    if (count < 0) {
        return;
    }
    sync_packages(items, item_count, results, count);
}

void section_de_wm(void) {
    PkgItem items[] = {
        {"hyprland",    ""},
        {"niri",        ""},
        {"mango",       ""},
        {"plasma-meta", ""},
        {"gnome",       ""},
        {"lxqt",        ""},
        {"cinnamon",    ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Desktop Environments / Window Managers",
                  "Select the DE(s) / WM(s) to install:", items, count);
}

void section_display_managers(void) {
    PkgItem items[] = {
        {"sddm",    ""},
        {"gdm",     ""},
        {"lightdm", ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Display Managers", "Select a login manager:", items, count);
}

void section_file_managers(void) {
    PkgItem items[] = {
        {"dolphin",    ""},
        {"nautilus",   ""},
        {"thunar",     ""},
        {"pcmanfm-qt", ""},
        {"yazi",       ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("File Managers", "Select file manager(s):", items, count);
}

void section_terminals(void) {
    PkgItem items[] = {
        {"kitty",     ""},
        {"alacritty", ""},
        {"ghostty",   ""},
        {"foot",      ""},
        {"konsole",   ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Terminal Emulators", "Select terminal(s):", items, count);
}

void section_editors(void) {
    PkgItem items[] = {
        {"neovim",       ""},
        {"vscodium-bin", ""},
        {"code",         ""},
        {"zed",          ""},
        {"micro",        ""},
        {"nano",         ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Text Editors", "Select editor(s):", items, count);
}

void section_browsers(void) {
    PkgItem items[] = {
        {"firefox",         ""},
        {"brave-bin",       ""},
        {"google-chrome",   ""},
        {"zen-browser-bin", ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Web Browsers", "Select browser(s):", items, count);
}

void section_shells(void) {
    PkgItem items[] = {
        {"zsh",             ""},
        {"zsh-completions", ""},
        {"fish",            ""},
        {"bash-completion", ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Shells & Terminal Frameworks", "Select shell(s) / framework(s):", items, count);
}

void section_widgets(void) {
    PkgItem items[] = {
        {"waybar",       ""},
        {"eww",          ""},
        {"ags",          ""},
        {"rofi-wayland", ""},
        {"wofi",         ""},
        {"dunst",        ""},
        {"hyprpaper",    ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("Status Bars / Shell Widgets", "Select widget(s) / launcher(s):", items, count);
}

void section_tools(void) {
    PkgItem items[] = {
        {"fastfetch",  ""},
        {"neofetch",   ""},
        {"htop",       ""},
        {"btop",       ""},
        {"cava",       ""},
        {"tty-clock",  ""},
        {"eza",        ""},
        {"bat",        ""},
        {"zoxide",     ""},
    };
    int count = sizeof(items) / sizeof(items[0]);
    pick_packages("System Tools & CLI Aesthetics", "Select tool(s):", items, count);
}

int install_selected_packages(void) {
    if (selected_count == 0) {
        run_msgbox("Nothing Selected", "You haven't selected any packages yet.");
        return 0;
    }

    char full_cmd[MAX_CMD_LEN];
    int  len = snprintf(full_cmd, sizeof(full_cmd), "%s", PACMAN_INSTALL);
    int  i;
    for (i = 0; i < selected_count && len < (int)sizeof(full_cmd); i++) {
        len += snprintf(full_cmd + len, sizeof(full_cmd) - len, "%s ", selected_packages[i]);
    }

    char confirm_text[MAX_CMD_LEN + 256];
    snprintf(confirm_text, sizeof(confirm_text),
             "%d package(s) selected.\n\nThe following command will run:\n\n%s\n\nProceed?",
             selected_count, full_cmd);

    if (!run_yesno("Confirm Installation", confirm_text)) {
        run_msgbox("Cancelled", "Installation cancelled. Nothing was changed.");
        return 0;
    }

    printf("\nRunning: %s\n\n", full_cmd);
    int result = system(full_cmd);

    if (result == 0) {
        run_msgbox("Success", "Installation finished successfully.");
    } else {
        run_msgbox("Error", "pacman reported an error. Check the terminal output above.");
    }

    return 1;
}

int main(void) {
    setenv("NEWT_COLORS",
        "root=white,black\n"
        "window=white,black\n"
        "border=white,black\n"
        "title=white,black\n"
        "textbox=white,black\n"
        "button=white,black\n"
        "actbutton=white,blue\n"
        "checkbox=white,black\n"
        "actcheckbox=white,blue\n"
        "listbox=white,black\n"
        "actlistbox=white,blue\n"
        "compactlistbox=white,blue\n"
        "entry=white,black\n",
        1);

    while (1) {
        char status_text[128];
        snprintf(status_text, sizeof(status_text),
                 "%d package(s) selected so far. Choose a category:", selected_count);

        PkgItem main_menu[] = {
            {"Desktop Environments", ""},
            {"Display Managers",     ""},
            {"File Managers",        ""},
            {"Terminals",            ""},
            {"Text Editors",         ""},
            {"Browsers",             ""},
            {"Shells",               ""},
            {"Widgets & Bars",       ""},
            {"CLI Tools",            ""},
            {"Install Selected",     ""},
            {"Exit",                 ""},
        };
        int item_count = sizeof(main_menu) / sizeof(main_menu[0]);

        char choice[MAX_PKG_LEN];
        int  ok = run_menu("Ultimate Arch Setup", status_text,
                            main_menu, item_count, choice);

        if (!ok) {
            if (run_yesno("Quit?", "Exit the builder without installing anything?")) {
                break;
            }
            continue;
        }

        if (strcmp(choice, "Desktop Environments") == 0) {
            section_de_wm();
        } else if (strcmp(choice, "Display Managers") == 0) {
            section_display_managers();
        } else if (strcmp(choice, "File Managers") == 0) {
            section_file_managers();
        } else if (strcmp(choice, "Terminals") == 0) {
            section_terminals();
        } else if (strcmp(choice, "Text Editors") == 0) {
            section_editors();
        } else if (strcmp(choice, "Browsers") == 0) {
            section_browsers();
        } else if (strcmp(choice, "Shells") == 0) {
            section_shells();
        } else if (strcmp(choice, "Widgets & Bars") == 0) {
            section_widgets();
        } else if (strcmp(choice, "CLI Tools") == 0) {
            section_tools();
        } else if (strcmp(choice, "Install Selected") == 0) {
            if (install_selected_packages()) {
                break;
            }
        } else if (strcmp(choice, "Exit") == 0) {
            break;
        }
    }

    return 0;
}
