#ifndef CONFIG_H_
#define CONFIG_H_

void config_set_game_path(char* p);
char* config_get_game_path();
char* config_get_asset_path(char* filename);
char* config_get_map_path(char* filename);

#endif // CONFIG_H_
