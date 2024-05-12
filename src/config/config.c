#include <stdio.h>
#include <stdlib.h>
#include <cwalk/cwalk.h>
#include <string.h>

#include "config.h"

#define ASSETS_DIR "assets"

static struct {
    char* game_path;
    char* assets_path;
} config;

void config_set_game_path(char* p) {
    config.game_path = p;
}

char* config_get_game_path() {
    return config.game_path;
}

char* config_get_asset_path(char* filename) {
    char buf[FILENAME_MAX];
    const char *paths[4];
    paths[0] = config.game_path;
    paths[1] = ASSETS_DIR;
    paths[2] = filename;
    paths[3] = NULL;
    cwk_path_join_multiple(paths, buf, sizeof(buf));
    return strdup(buf);;
}

char* config_get_map_path(char* filename) {
    char buf[FILENAME_MAX];
    const char *paths[4];
    paths[0] = config.game_path;
    paths[1] = "maps";
    paths[2] = filename;
    paths[3] = NULL;
    cwk_path_join_multiple(paths, buf, sizeof(buf));
    return strdup(buf);;
}
