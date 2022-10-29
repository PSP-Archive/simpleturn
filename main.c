#include "pg.h"

#define SCOREFILE_PATH "fatms0:/PSP/GAME/SIMPLETURN/HIGHSCORE.DAT"

static int high_score;

unsigned short rgb2col(unsigned char r,unsigned char g,unsigned char b) {
	return ((((b>>3) & 0x1F)<<10)+(((g>>3) & 0x1F)<<5)+(((r>>3) & 0x1F)<<0)+0x8000);
}

void wait_button(void) {
	ctrl_data_t ctl;
	int btn;

	btn=1;
	while(btn!=0){
		pgWaitV();
		sceCtrlRead(&ctl,1);
		btn = ((ctl.buttons & 0xF000) != 0);
	}
	btn=0;
	while(btn==0){
		pgWaitV();
		sceCtrlRead(&ctl,1);
		btn = ((ctl.buttons & 0xF000) != 0);
	}
}

// 数値表示
void DisplayScore(int x,int y,int back_col,int col,int num) {
	unsigned char str[4];
	int px,py,n;
	unsigned short *vr;

	for(py=0;py<8;py++) {
		vr = (unsigned short *)pgGetVramAddr(x*8,y*8+py);
		for(px=0;px<24;px++) *vr++ = back_col;
	}

	n = (num / 100) % 10;
	if (n==0) { str[0] = ' '; } else { str[0] = '0'+n; }
	n = (num / 10) % 10;
	if (n==0) { str[1] = ' '; } else { str[1] = '0'+n; }
	str[2] = '0'+(num % 10);
	str[3] = 0;
	pgPrint(x,y,col,str);
}

// Hiscore
void ReadHighScore(void) {
	int fd;
	fd = sceIoOpen(SCOREFILE_PATH,O_RDWR);
	if (fd>=0) {
		sceIoRead(fd, (void *)&high_score, sizeof(int));
		sceIoClose(fd);
	} else {
		pgFillvram(rgb2col(0,0,0));
		pgPrint(0,0,rgb2col(255,0,0),"ERROR : CANNOT READ");
		pgPrint(0,1,rgb2col(255,0,0),SCOREFILE_PATH);
		wait_button();
		pgFillvram(rgb2col(0,0,0));
	}
}

void WriteHighScore(void) {
	int fd;
	fd = sceIoOpen(SCOREFILE_PATH,O_RDWR);
	if (fd>=0) {
		sceIoWrite(fd, (void *)&high_score, sizeof(int));
		sceIoClose(fd);
	}
}

// タイトル画面
void Title(void) {
	pgWaitV();
	pgFillvram(rgb2col(0,0,0));
	pgPrint(20,12,0xffff,"S i m p l e  T u r n");
	pgPrint(22,24,0xffff,"High Score : ");
	DisplayScore(35,24,rgb2col(0,0,0),0xffff,high_score);
	wait_button();
}

// ゲーム
void Game(void) {
	int				x,y,my_x,my_y,btn,flg,dir,score;
	unsigned short	*vr,*vr2,col;
	int				back_col;
	ctrl_data_t		ctl;

	// 初期化
	my_x     = 0;
	my_y     = 0;
	dir      = 0;
	score    = 0;
	back_col = rgb2col(0,0,64);

	// ゲームフィールド描画
	pgWaitV();
	pgFillvram(back_col);
	col = rgb2col(128,128,128);
	for(y=0;y<8;y++) {
		vr = (unsigned short *)pgGetVramAddr(0,y);
		for(x=0;x<SCREEN_WIDTH;x++) *vr++ = col;
	}
	for(y=8;y<(SCREEN_HEIGHT-16);y++) {
		vr  = (unsigned short *)pgGetVramAddr(0,y);
		vr2 = (unsigned short *)pgGetVramAddr(SCREEN_WIDTH-8,y);
		for(x=0;x<8;x++) { *vr++ = col; *vr2++ = col; }
	}
	for(y=0;y<8;y++) {
		vr = (unsigned short *)pgGetVramAddr(0,(SCREEN_HEIGHT-16)+y);
		for(x=0;x<SCREEN_WIDTH;x++) *vr++ = col;
	}
	pgPrint(49,33,0xffff,"Score : ");
	DisplayScore(57,33,back_col,0xffff,score);

	//
	sceCtrlRead(&ctl,1);
	btn   = ((ctl.buttons & 0xF000) != 0);
	flg   = btn;

	//
	while(1) {
		// 自機描画
		col = rgb2col(255,255,255);
		for(y=0;y<8;y++) {
			vr  = (unsigned short *)pgGetVramAddr(my_x*8+8,my_y*8+8+y);
			for(x=0;x<8;x++) *vr++ = col;
		}

		// ボタン判定
		sceCtrlRead(&ctl,1);
		btn = ((ctl.buttons & 0xF000) != 0);
		if (btn) {
			if (!flg) {
				dir = (dir+1) % 4;
				score++;
				DisplayScore(57,33,back_col,0xffff,score);
			}
			flg = 1;
		} else {
			flg = 0;
		}

		// 進行
		switch(dir) {
		case 0:		my_x++; break;
		case 1:		my_y++; break;
		case 2:		my_x--;	break;
		default:	my_y--;	break;
		}

		// 衝突判定
		vr  = (unsigned short *)pgGetVramAddr(my_x*8+8,my_y*8+8);
		if (*vr != back_col) break;

		// VSync待ち
		pgWaitV();
	}

	// ゲームオーバー
	pgWaitV();
	pgPrint4(3,3,rgb2col(255,255,0),"GAME OVER");
	if (high_score < score) {
		pgPrint2(6,9,rgb2col(255,0,0),"--- HIGH SCORE! ---");
		high_score = score;
		WriteHighScore();
	}
	pgWaitVn(30);
	pgPrint(23,21,rgb2col(255,255,0),"PRESS ANY KEY");
	
	wait_button();
}



int xmain(void)
{
	// Initialize
	pgInit();
	pgScreenFrame(1,0);
	sceCtrlInit(0);
	sceCtrlSetAnalogMode(0);

	high_score = 0;
	ReadHighScore();

	while(1) {
		Title();
		Game();
	}

	return 0;
}
