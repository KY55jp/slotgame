#pragma once

#include <cstdint>
#include "pico_display.hpp"

using namespace pimoroni;


struct character_data_def {
	Rect r;
	uint16_t* bmp_data;
	bool move;
	int speed;
	int char_no;
};

enum game_status_tag { //ゲームの状態
	Game_Opening, //オープニング画面
	Game_OnGoing, //ゲーム進行中
	Game_OnScore, //得点判定
	Game_OnPause,
	Game_Over
};

extern game_status_tag game_status;

extern int score;
extern int coin;
extern int stop_slot_pos;
extern int start_speed;

extern const uint16_t open_char_data[];
extern const uint16_t char_data2[];

extern PicoDisplay pico_display;

#define START_SPEED 1

void draw_character(Rect& r, const uint16_t data[], int scale);
void init();
void rotate_slot(character_data_def *p, int size);
void draw_bg();
void shuffle_slot(int* data, int size);
void init_data(std::vector<character_data_def>& cds);
void draw_slot(std::vector<character_data_def> &cds, int scale);
bool display_opening(std::vector<character_data_def>& cds);
void do_gameover(std::vector<character_data_def>& cds);
int collision_detection(std::vector<character_data_def>& cds);
int do_process(std::vector<character_data_def>& cds, int scale);
void do_pause();

