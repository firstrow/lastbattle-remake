#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "map/map.h"

#define MAXCHAR 1024

static int ground[] = {65, 66, 67, 75, 76, 77};
static int shores[] = {1, 2, 3, 10, 11, 20, 32, 35, 59, 58, 68, 88, 86, 64, 71, 72, 81, 82};

static char *rtrim(char *const s)
{
        size_t len;
        char *cur;

        if(s && *s) {
                len = strlen(s);
                cur = s + len - 1;

                while(cur != s && isspace(*cur))
                        --cur, --len;

                cur[isspace(*cur) ? 0 : 1] = '\0';
        }

        return s;
}

static int to_terrain_type(int idx) {
    for (int i = 0; i < sizeof(ground) / sizeof(ground[0]); i++) {
        if (ground[i] == idx) {
            return MAP_TILE_GROUND;
        }
    }

    for (int i = 0; i < sizeof(shores) / sizeof(shores[0]); i++) {
        if (shores[i] == idx) {
            return MAP_TILE_SHORE;
        }
    }

    return MAP_TILE_WATER;
}

void map_load_csv(char* fpath) {
    char row[MAXCHAR];
    char *token;

    map_reset();

    int x = 0;
    int y = 0;

    FILE *fp;
    fp = fopen(fpath, "r");

    while(feof(fp) != true) {
        fgets(row, MAXCHAR, fp);

        x = 0;
        if (y >= MAP_HEIGHT)
            break;

        token = strtok(row, ",");
        map_set_tileset_idx(atoi(token), x, y);
        map_set_terrain_type(to_terrain_type(atoi(token)), x, y);

        while(token != NULL) {
            x++;
            token = strtok(NULL, ",");
            if (token != NULL)  {
                rtrim(token);
                map_set_tileset_idx(atoi(token), x, y);
                map_set_terrain_type(to_terrain_type(atoi(token)), x, y);
            }
        }

        y++;
    }

    fclose(fp);
}
