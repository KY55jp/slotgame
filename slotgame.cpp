/*
 * スロットゲーム for ラズパイPICO
 * copyright (c) 2021 KoKoNa
 */
#include "slotgame.hpp"
#include "pico_display.hpp"
#include "time.h"

using namespace pimoroni;

struct character_data_def2 {
	Rect r;
	uint16_t* bmp_data;
};

/* スロットデータ定義 */
std::vector<character_data_def> cds;
std::vector<character_data_def2> cds2;
int scale = 2; //描写スケール 1:通常　2以上:　倍率

/* データ初期化 */
void init_data2(std::vector<character_data_def2>& cds)
{
	//すでにデータがセットされている場合、データを削除
	if(!cds.empty()){
		cds.clear();
	}

	/* データ初期化 */
	for(int i = 0; i < 3; i++){
		character_data_def2 cd;
		cd.r = Rect(0, 32, 32, 32);
		cd.bmp_data = (uint16_t *)&char_data2[i * 32 * 32];
		cds.push_back(cd);
	}
}

//オープニング画面 戻り値:true->slot false->janken
bool display_opening(std::vector<character_data_def>& cds)
{
	pico_display.set_pen(0, 0, 0);
	pico_display.clear();
	pico_display.set_pen(255, 255, 255);
	
	Rect r(0,0,32,32);
	draw_character(r, open_char_data, 3);
//	Rect r2(32,0,32,32);
//	draw_character(r2, &open_char_data[32 * 32], 3);

//	pico_display.text("COCONA's Janken", Point(0, 20), 240, 3);
//	pico_display.set_pen(255, 255, 255);
//	pico_display.text("PRESS X TO START", Point(0, 70), 240, 2);
	pico_display.set_pen(255, 255, 255);
	pico_display.text("Press X TO START JANKEN", Point(0, 100), 240, 2);
	pico_display.text("Press Y TO START SLOT", Point(0, 122), 240, 2);
	pico_display.update();
	bool slot, janken;
	do {
		slot = pico_display.is_pressed(pico_display.Y);
		janken = pico_display.is_pressed(pico_display.X);
	} while(!slot && !janken);
	
	game_status = Game_OnGoing;

	srandom((unsigned int)time(NULL));/* 乱数種を初期化 */
	bool result = false;
	if(slot){
		init_data(cds); //データ初期化
		result = true;
	}else{
		init_data2(cds2); //データ初期化
	}

	game_status = Game_OnGoing;
	return result;
}

//ゲームオーバー画面
void do_gameover(std::vector<character_data_def>& cds)
{
	pico_display.set_pen(0, 0, 0);
	pico_display.clear();
	pico_display.set_pen(0, 0, 255);
	pico_display.text("GAME OVER", Point(20, 20), 240, 3);
	pico_display.text("PRESS B TO RETRY", Point(20, 60), 240, 3);
	pico_display.update();
	while(!pico_display.is_pressed(pico_display.B)){} //「B」キーが押されるまで、ここでウェイト
	init_data(cds); //データ初期化
	game_status = Game_Opening;
	score = 0;
	coin = 200;
	start_speed = START_SPEED;
}

/* 背景描写 */
void draw_bg2()
{
	char buf[256];
	pico_display.set_pen(255,255,255);
	sprintf(buf,"X: Goo- Y: Choki- B: Pa-");
	pico_display.text(buf, Point(0, 0), 240, 2);
	sprintf(buf,"A: RETURN Opening");
	pico_display.text(buf, Point(0, 18), 240, 2);
	pico_display.text("You         Com", Point(0, 44), 240, 2);
}

void draw_janken(std::vector<character_data_def2> &cds, int scale, int char_no, int com)
{
	/* イメージの描写 */
	character_data_def2 cd = cds[char_no];
	draw_character(cd.r, cd.bmp_data, scale); //自分のじゃんけん
	Rect r(40, 32, 32, 32);
	draw_character(r, cds2[com].bmp_data, scale); //コンピュータのじゃんけん
}

int do_process2(std::vector<character_data_def2>& cds, int scale)
{
	//ディスプレイ更新
	pico_display.set_pen(0, 0, 0);
	pico_display.clear();

	draw_bg2();

	pico_display.update(); //ディスプレイ更新
	
	//何かキーが押されるまでループ
	bool x, y, b, a;
	do {
		x = pico_display.is_pressed(pico_display.X);
		y = pico_display.is_pressed(pico_display.Y);
		b = pico_display.is_pressed(pico_display.B);
		a = pico_display.is_pressed(pico_display.A);
	} while(!y && !x && !b && !a);

	int janken = 0;
	if(x){
		janken = 0;
	} else if(y){
		janken = 1;
	} else if(b){
		janken = 2;
	} else if(a){
		game_status = Game_Opening;
	}

	int com = random() % 3; //コンピュータの手番

	pico_display.set_pen(255, 0, 0);
	if(janken == com){
		pico_display.text("DRAW", Point(90, 20), 240, 3);
	} else if((janken == 2 && com == 0)
		  || (janken == 0 && com == 1)
		  || (janken == 1 && com == 2))
	{
		pico_display.text("YOU WIN", Point(90, 20), 240, 3);
	} else {
		pico_display.text("YOU LOSE", Point(90, 20), 240, 3);
	}
	draw_janken(cds, scale, janken, com); /* スロット描写 */
	pico_display.update(); //ディスプレイ更新
	sleep_ms(2000);
	
	return 1;
}

//メイン処理。ここから処理が開始される
int main()
{
	init(); //初期化処理
	
	//int characters = 9;

	int score_result = 0;
	bool slot_game = false;
	while(true) {
		switch(game_status){
		case Game_Opening:
			slot_game = display_opening(cds);
			break;
		case Game_OnGoing:
			if(slot_game){
				score_result = do_process(cds, scale);
			}else{
				score_result = do_process2(cds2, scale);
			}
			break;
		case Game_OnScore:
			game_status = Game_OnGoing;
			for(int i = 0; i < score_result; i++){
				pico_display.set_led(255, 255, 255);
				sleep_ms(500);
				pico_display.set_led(0, 0, 0);
				sleep_ms(500);
			}
			init_data(cds); //データ初期化
			break;
		case Game_OnPause:
			do_pause();
			break;
		case Game_Over:
			for(int i = 0; i < 3; i++){
				pico_display.set_led(255, 0, 0);
				sleep_ms(500);
				pico_display.set_led(0, 0, 0);
				sleep_ms(300);
			}
			do_gameover(cds);
			break;
		}
	}
}
