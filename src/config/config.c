#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "../logger/logger.h"
#include "../global.h"

int mk_config_path(const char *path, mode_t mode) {
    char temp[4096];
    char *p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '/') {
        // Null terminate trailing / in directory string
        temp[len - 1] = '\0';
    }
    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, mode) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }
    if (mkdir(temp, mode) != 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

char *cfg_path(const char *led_cfg) {
    const char *xdg_config = getenv("XDG_CONFIG_HOME");
    static char path[4096];

    if (xdg_config && xdg_config[0] != '\0') {
        if (led_cfg) {
            snprintf(path, sizeof(path), "%s/%s", xdg_config, led_cfg);
        } else {
            snprintf(path, sizeof(path), "%s", xdg_config);
        }
        return path;
    }

    const char *home = getenv("HOME");
    if (!home) {
        return NULL;
    }

    if (led_cfg) {
        snprintf(path, sizeof(path), "%s/.config/%s", home, led_cfg);
    } else {
        snprintf(path, sizeof(path), "%s/.config", home);
    }

    return path;
}

static char *sanitize(char *val) {
    if (!val) return NULL;
    while (isspace((unsigned char)*val)) val++;

    char *end = val + strlen(val) - 1;
    while (end > val && (isspace((unsigned char)*end) || *end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }

    if (*val == '"') {
        val++;
        end = val + strlen(val) - 1;
        if (end >= val && *end == '"') {
            *end = '\0';
        }
    }
    return val;
}

int load_config(LED_Window *window) {
    // This section will likely be way more heavily commented as this is one of the more complicated aspects in the beggining of writing.
    char *config_dir = cfg_path("led");
    l_debug("Using config path: %s", config_dir);
    if (mk_config_path(config_dir, 0700) != 0) {
        l_error("Couldn't create config directory. Check user permissions for the .config folder.");
        return 1;
    }
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/.ledrc", config_dir);
    FILE *f = fopen(full_path, "r");
    if (!f) return 1;
    char buffer[512];
    size_t line_num = 0;

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        line_num++;

        // Strip all comments out
        char *comment = strstr(buffer, "//");
        if (comment) *comment = '\0';

        // Key-Value separator
        char *sep = strstr(buffer, CFG_DELIMITER_SET);
        if (!sep) continue;

        *sep = '\0';
        char *full_key = buffer;
        char *val_raw = sep + 2;

        char *val = sanitize(val_raw);
        if (!val || *val == '\0') {
            l_warn("[LINE %zu] :: Key '%s' present with no set value.", line_num, full_key);
            continue;
        }

        if (strcmp(full_key, "isModal") == 0) {
            if (strcmp(val, "modal") == 0) {
                window->isModal = 1;
                l_debug("Key 'isModal' set to 'modal'.");
            } else if (strcmp(val, "multiModal") == 0) {
                window->isModal = 1;
                window->isMultiModal = 1;
                l_debug("Key 'isModal' set to 'multiModal'.");
            } else {
                l_warn("[LINE %zu] :: Incorrect value '%s' for 'isModal'. Ignoring.");
                window->isModal = 0;
            }
            continue;
        }

        char *dot = strchr(full_key, CFG_DELIMITER_SUB);
        if (dot) {
            *dot = '\0';
            char *sect = full_key;
            char *prop = dot + 1;

            if (strcmp(sect, "font") == 0) {
                if (strcmp(prop, "Family") == 0) {
                    size_t len = strlen(val) + 1;
                    window->fontFamily = malloc(len);
                    snprintf(window->fontFamily, len, "%s", val);
                    l_debug("font.Family set to %s", window->fontFamily);
                } else if (strcmp(prop, "Size") == 0) {
                    window->fontSize = atoi(val);
                    l_debug("font.Size set to %d", window->fontSize);
                } else {
                    l_warn("[LINE %zu] :: Unknown 'font' property '%s'.", line_num, prop);
                }
            }
        }
    }
    fclose(f);
    return 0;
}
