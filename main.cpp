#include <stdio.h>
#include <graphics.h>//安装easyx图像库，easyx图形库的头文件
#include <time.h>
#include <math.h>
#include "tools.h"
#include "vector2.h"//抛物线运动的工具
#include <mmsystem.h>//音乐库
#pragma comment(lib,"winmm.lib")//告诉链接器将winmm.lib库文件链接到程序中

/*
*数据宏定义
*/
#define WIN_WIDTH 900//窗口的高度、宽度
#define WIN_HEIGHT 600
#define ZHI_WU_WIDTH 52 //植物卡牌的宽度
#define ZHI_WU_HEIGHT 52*1.28 //宽：高=1.28
#define MAP_WIDTH 81 //植物种植的地图的行距和列距
#define MAP_HEIGHT 102
#define ZM_MAX 10
/*
*枚举存储游戏状态，每一个变量代表了游戏的当前状态
*/
enum { GOING, WIN, FAIL };
int killCount;//已经杀掉的僵尸个数
int zmCount;//已经出现的僵尸个数
int gameStatus;//游戏当前状态

/*
*枚举存储植物卡牌，最后一个ZHI_WU_COUNT代表卡牌数量
*/
enum ZW { WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT };

/*
* 变量声明
*/
IMAGE imgBg;//表示背景图片
IMAGE imgBar;//表示植物选择栏
IMAGE imgCards[ZHI_WU_COUNT];//植物卡牌的数组
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];//植物的帧数图

/*
*创建植物地块数据类型
*/
struct zhiwu {
	int type;		//0：没有植物；1：第一种植物
	int frameIndex;//序列帧的下标

	int catched;//是否被僵尸捕获
	int deadTime;//死亡计数器 实现较为简单

	int timer;//阳光计时器
	int x, y;
};
//该结构代表某个地块是否种植植物
struct zhiwu map[5][9];//植物地图

int curX, curY;//当前选中的植物在移动过程中的坐标
int curZhiWu;//当选选中的植物，0表示没有选中

/*
*枚举存储阳光，最后一个ZHI_WU_COUNT代表卡牌数量
*/
enum SUNSHINE { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };

/*
*创建阳光数据类型
*/
struct sunshineBall {

	int frameIndex;//当前的图片帧的序号
	int used;//是否在使用
	int mytimer;//定时器

	//以下代码已被优化
	int x, y;//阳光球在飘落过程中的坐标位置（x不变）
	int dextY;//飘落的目标位置的坐标
	float xoff;
	float yoff;//xy轴的偏移量

	float t;//贝塞尔曲线的时间点0..1
	vector2 p1, p2, p3,p4;//p1和p4分别是起点和终点；p2、p3是两个控制点
	vector2 pCur;//当前时刻阳光球的位置
	float speed;//阳光球的速度
	int status;//阳光球的状态
};
struct sunshineBall balls[10];//内存池
IMAGE imgSunshineBall[29];//阳光帧数图数组
int ballMax = sizeof(balls) / sizeof(balls[0]);//阳光的最大数量
int sunShine;//阳光收集数

/*
*枚举存储僵尸种类，最后一个JIANG_SHI_COUNT代表卡牌数量
*/
enum ZM{ JU_QI, PU_TONG, JIANG_SHI_COUNT};

/*
*创建僵尸数据类型
*/
struct zm {
	int x, y;
	int frameIndex;//帧下标
	int used;
	int speed;
	int row;//僵尸在哪一行
	int blood;
	int dead;
	int eating;//正在吃植物的状态
};
struct zm zms[10];//僵尸数量
IMAGE* imgZM[JIANG_SHI_COUNT][20];//僵尸的帧数图
int zmMax = sizeof(zms) / sizeof(zms[0]);//僵尸的最大数量
IMAGE imgZMDead[12];//僵尸死亡图片
IMAGE imgZMD_Head[12];//僵尸死亡头图片
IMAGE* imgZMEat[JIANG_SHI_COUNT][21];//僵尸吃的帧图片
IMAGE imgZMStand[14];//僵尸站立的图片

/*
*子弹的数据类型
*/
struct bullet
{
	int x, y;
	int row;//行
	int used;
	int speed;
	int blast;//是否射中僵尸
	int frameIndex;//帧序号
};
struct bullet bullets[30];//子弹内存池
IMAGE imgBulletNormal;//正常子弹的图片
IMAGE imgBullBlast[4];//子弹碰撞的图片
int bulletMax = sizeof(bullets) / sizeof(bullets[0]);//子弹的最大数量

/*
* 函数声明
*/
void gameInit();/*游戏初始化*/
void updateWindow();/*更新窗口*/
void userClick();/*用户的点击功能*/
void updateGame();/*更新游戏数据*/
void startUI();/*启动菜单*/
void createSunshine();/*创建阳光*/
void updateSunshine();/*阳光状态更新*/
void collectSunshine(ExMessage*);/*阳光收集功能*/
void XYchange(int, int, float*, float*);//偏移量计算函数
void createZM();/*创建僵尸*/
void updateZM();/*僵尸状态更新*/
void drawZM();/*绘制僵尸*/
void shoot();//发送豌豆子弹
void updateBullets();//更新子弹
void collisionCheck();//实现碰撞检测
void checkBullet2ZM();/*实现豌豆子弹和僵尸的碰撞检测*/
void checkZm2ZhiWu();/*实现僵尸吃植物的检测*/
void drawsunshine();/*绘制阳光*/
void updateZhiWu();/*更新植物的数据*/
void drawCards();/*渲染卡片*/
void drawmap();/*渲染植物卡片种植情况*/
void drawBullets();/*渲染子弹*/
void viewScence();//转场
void barsDown();//工具栏的下降特效
int checkOver();//检查游戏状态

int main(void) {
	gameInit(); //游戏数据加载函数
	startUI();//菜单加载函数
	viewScence();//转场
	barsDown();//工具栏的下降特效

	int timer = 0;//计数器
	int flag = 1;//是否刷新游戏数据的标志
	while (1) {
		userClick();//点击事件
		timer += getDelay();//获取一个延时时间
		if (timer > 70) {
			flag = 1;
			timer = 0;
		}
		if (flag) {
			flag = 0;
			updateWindow();
			updateGame();
			checkOver();//检查游戏状态
			if (checkOver()) break;
		}
		
	}
	system("pause");
	return 0;
}

int checkOver() {
	int ret = 0;
	IMAGE imgstatus;
	if (gameStatus == WIN) {
		Sleep(1000);

		loadimage(&imgstatus, "res/win.png", 613 * 0.5, 399 * 0.5);
		putimagePNG(350, 200, &imgstatus);

		ret = 1;
	}
	else if(gameStatus == FAIL){
		Sleep(1000);

		loadimage(&imgstatus, "res/fail.png", 564 * 0.5, 468 * 0.5);
		putimagePNG(350, 200, &imgstatus);

		ret = 1;
	}
	return ret;
}

void barsDown() {
	int height = imgBar.getheight();
	for (int y = -height; y <= 0; y++) {
		BeginBatchDraw();

		putimage(-112, 0, &imgBg);
		putimagePNG(150, y, &imgBar);
		for (int i = 0; i < ZHI_WU_COUNT; i++) {
			int x = 220 + i * ZHI_WU_WIDTH;
			//int y = 6 + y;
			putimage(x, 6 + y, &imgCards[i]);
		}
		EndBatchDraw();
		Sleep(0.5);
	}
}

void viewScence() {
	//背景是1398*600,之前初始化时进行过缩放
	int xMin = WIN_WIDTH - imgBg.getwidth();//900-1398
	vector2 points[9] = {
		//因为数据量小，直接输入位置
		{550, 80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340} };
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 14;
	}
		
	int count = 0;
	//片场巡回
	for (int x = 0; x >= xMin; x -= 2) {
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x,
				points[k].y,
				&imgZMStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 14;
			}
		}
		if (count >= 10) count = 0;
		EndBatchDraw();
		Sleep(2);
	}

	//停留2s左右
	count = 0;
	for (int i = 0; i < 100; i++) {
		BeginBatchDraw();

		putimage(xMin, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x, points[k].y, &imgZMStand[index[k]]);
			if (count > 2) {
				index[k] = (index[k] + 1) % 14;
			}
		}
		if (count > 2) count = 0;
		EndBatchDraw();
		Sleep(20);
	}

	//切回游戏画面
	for (int x = xMin; x <= -112; x += 2) {
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x,
				points[k].y,
				&imgZMStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 14;
			}
		}
		if (count >= 10) count = 0;
		EndBatchDraw();
		Sleep(2);
	}
}

void gameInit() {
	//加载游戏背景图片,,高：宽=2.33
	//如果loadimage报错，将字符集改为“多字节字符集” 属性->高级
	int Bg_height = WIN_HEIGHT;
	loadimage(&imgBg, "res/Bg.jpg", Bg_height * 2.33, Bg_height);
	//把植物播放图片的数组的内存设置为0
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));//地图初始化

	//游戏状态的初始化
	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;

	//初始化植物卡牌,植物选择栏高：宽=6.10
	int Bar_height = 80;
	loadimage(&imgBar, "res/Bar.png", Bar_height * 6.10, Bar_height);
	char name[64];//保存图片的路径
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//生成植物卡牌的文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name, ZHI_WU_WIDTH, ZHI_WU_HEIGHT);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//先判断图片文件是否存在，再加载
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else break;
		}
	}

	curZhiWu = 0;//选择的植物的序号
	sunShine = 125;//初始阳光数

	//初始化阳光图片数组
	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}
	//配置随机种子
	srand(time(NULL));
	//创建游戏的图像窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//设置阳光数字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 25;
	f.lfWidth = 10;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);//设置字体操作
	setbkmode(TRANSPARENT);//背景透明
	setcolor(BLACK);//字体颜色

	//初始化僵尸
	memset(imgZM, 0, sizeof(imgZM));//把僵尸播放图片的数组的内存设置为0
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < JIANG_SHI_COUNT; i++) {
		for (int j = 0; j < 20; j++) {
			//生成僵尸帧数的图片名
			sprintf_s(name, sizeof(name), "res/zm/%d/%d.png", i, j + 1);
			//先判断图片文件是否存在，再加载
			if (fileExist(name)) {
				imgZM[i][j] = new IMAGE;
				loadimage(imgZM[i][j], name);
			}
			else break;
		}
	}

	//子弹
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//初始化豌豆子弹的帧图片数组
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png",
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k, true);
	}

	//僵尸死亡的初始化
	for (int i = 0; i < 12; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
		sprintf_s(name, sizeof(name), "res/zm_dead_head/%d.png", i + 1);
		loadimage(&imgZMD_Head[i], name);
	}

	//僵尸吃动作的初始化
	memset(imgZMEat, 0, sizeof(imgZMEat));//把僵尸图片的数组的内存设置为0
	for (int i = 0; i < JIANG_SHI_COUNT; i++) {
		for (int j = 0; j < 21; j++) {
			//生成僵尸帧数的图片名
			sprintf_s(name, sizeof(name), "res/zm_eat/%d/%d.png", i, j + 1);
			//先判断图片文件是否存在，再加载
			if (fileExist(name)) {
				imgZMEat[i][j] = new IMAGE;
				loadimage(imgZMEat[i][j], name);
			}
			else break;
		}
	}
	//僵尸站立的初始化
	for (int i = 0; i < 14; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZMStand[i], name);
	}
}


void updateWindow() {
	BeginBatchDraw();//easyx中的函数，开始缓冲

	//渲染游戏界面
	putimage(-112, 0, &imgBg);
	//putimage(250, 0, &imgBar);用边框处理方法让界面更好看
	putimagePNG(150, 0, &imgBar);
	drawCards();

	//渲染目前植物种植情况
	drawmap();

	//渲染阳光分数
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);
	outtextxy(175, 55, scoreText);//输出分数

	//渲染僵尸
	drawZM();

	//渲染子弹图片
	drawBullets();

	//渲染拖动过程中的植物
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		//curX-img->getwidth()/2让光标在图片正中央
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	//渲染阳光
	drawsunshine();

	EndBatchDraw();//easyx中的函数，结束双缓冲
}

void drawBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {
				//子弹击中的渲染
				IMAGE* img = &imgBullBlast[bullets[i].frameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);
			}
			else {
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}
	}
}

void drawmap() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0) {
				//以下代码已被优化
				//int x = 250 + j * MAP_WIDTH + 10;
				//int y = ZHI_WU_HEIGHT + i * MAP_HEIGHT + 10;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				//以下代码已被优化
				//putimagePNG(x, y, imgZhiWu[ZhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[ZhiWuType][index]);
			}
		}
	}
}

void drawCards() {
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//第一张卡牌距离窗口左边界的距离为70
		int x = 220 + i * ZHI_WU_WIDTH;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

void drawsunshine() {
	for (int i = 0; i < ballMax; i++) {
		//以下代码已被优化
		//if (balls[i].used || balls[i].xoff != 0) {
		if (balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			//以下代码已被优化
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}
}

void userClick() {
	ExMessage msg;//消息类型
	static int status = 0;//记录状态

	if (peekmessage(&msg)) {
		//判断当前有无输入的消息
		if (msg.message == WM_LBUTTONDOWN) {
			curX = msg.x;
			curY = msg.y;
			//鼠标按下
			if (msg.x > 220 && msg.x < 220 + ZHI_WU_WIDTH * ZHI_WU_COUNT && msg.y < ZHI_WU_HEIGHT) {
				//鼠标选择范围在植物卡牌工具栏范围内,index刚好是下标
				int index = (msg.x - 220) / ZHI_WU_WIDTH;
				if (index == WAN_DOU && sunShine < 100) return;
				if (index == XIANG_RI_KUI && sunShine < 50) return;
				status = 1;
				curZhiWu = index + 1;
			}
			else {//收集阳光
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			//鼠标移动
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {
			//鼠标放开
			//植物种植地块高102 宽81
			if (curZhiWu == 1) sunShine -= 100;
			if (curZhiWu == 2) sunShine -= 50;

			if (msg.x > 140 && msg.y > ZHI_WU_HEIGHT && msg.y < WIN_HEIGHT - 10) {
				int row = (msg.y - ZHI_WU_HEIGHT) / MAP_HEIGHT;
				int col = (msg.x - 140) / MAP_WIDTH;
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
					//以下代码已被优化
					//int x = 250 + j * MAP_WIDTH + 10;
				    //int y = ZHI_WU_HEIGHT + i * MAP_HEIGHT + 10;
					map[row][col].x = 140 + col * MAP_WIDTH + 10;
					map[row][col].y= ZHI_WU_HEIGHT + row * MAP_HEIGHT + 10;
				}
			}
			curZhiWu = 0;
			status = 0;
		}
	}
}

void updateGame() {
	//更新植物的种植情况
	updateZhiWu();

	createSunshine();//创建阳光
	updateSunshine();//更新阳光状态

	createZM();//创建僵尸
	updateZM();//更新僵尸状态

	shoot();//发送豌豆子弹
	updateBullets();//更新子弹

	collisionCheck();//实现豌豆子弹和僵尸的碰撞检测
}

void updateZhiWu() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int zhiWuType = (map[i][j].type) - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}

void startUI() {
	IMAGE imgBg, imgMenu1, imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;//按钮是否选中
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
		EndBatchDraw();

		ExMessage msg;//按钮状态
		if (peekmessage(&msg)) {
			//点中让按钮发亮
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 475 && msg.x < 475 + 300 &&
				msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
				putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
			}
			//已经点中过按钮，此时又放开
			else if (msg.message == WM_LBUTTONUP && flag) {
				flag = 0;
				putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
				int timer = 0;
				while (timer < 100) {
					timer += getDelay();//获取一个延时时间
				}
				return;
			}
		}
	}
}

void createSunshine() {
	static int count = 0;//阳光产生计数，不能太频繁
	static int fre = 50;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 200;//100-299
		count = 0;

		//从阳光池子取一个可以使用的
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		//更新阳光数据
		balls[i].used = 1;
		balls[i].frameIndex = 0;
		balls[i].mytimer = 0;

		//以下代码已被优化
		//balls[i].x = 250 + rand() % (800 - 250);//阳光x的范围是250-800
		//balls[i].y = 0;
		//balls[i].dextY = 60 + (rand() % 5) * MAP_HEIGHT;//阳光y的范围是60-600
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;//xy轴的偏移量

		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(140 + rand() % (800 - 140), 0);
		balls[i].p4 = vector2(balls[i].p1.x, 60 + (rand() % 5) * MAP_HEIGHT);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}

	//向日葵生产阳光
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == XIANG_RI_KUI + 1) {
				map[i][j].timer++;
				if (map[i][j].timer > 200) {
					map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax) return;
					balls[k].used = 1;
					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
					int w = (50 + rand() % 50) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w,
						map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() - 
						imgSunshineBall[0].getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 30);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 30);
					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.10;//10帧到达目的地
					balls[k].t = 0;
				}
			}
		}
	}
}

void updateSunshine() {
	for (int i = 0; i < ballMax; i++) {
		//如果这个阳光已经被使用
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				//p1 + t * (p4 -p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->mytimer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].mytimer++;
				if (balls[i].mytimer > 100) {
					balls[i].used = 0;
					balls[i].mytimer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sun->used = 0;
					sunShine += 25;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->mytimer = 0;
				}
			}

			//balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			//if(balls[i].mytimer==0)
			//	balls[i].y += 2;
			//if (balls[i].y >= balls[i].dextY) {
			//	balls[i].mytimer++;
			//	//阳光存在的时间超过规定时间
			//	if(balls[i].mytimer>100)
			//		balls[i].used = 0;
			//}
		}
		//else if (balls[i].xoff != 0) {
		//	//偏移量因为是浮点数可能不准确 因此每次都进行计算
		//	XYchange(balls[i].x, balls[i].y, &balls[i].xoff, &balls[i].yoff);
		//	balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
		//	balls[i].x -= balls[i].xoff;
		//	balls[i].y -= balls[i].yoff;
		//	if (balls[i].y < 0 || balls[i].x < 220) {
		//		balls[i].xoff = 0;
		//		balls[i].yoff = 0;
		//		sunShine += 25;
		//	}
		//}
	}
}

void XYchange(int x, int y, float* n, float* m) {
	//计算偏移量
	float destY = 5;
	float destX = 260;
	float angle = atan((y - destY) / (x - destX));
	*n = 6 * cos(angle);
	*m = 6 * sin(angle);
}

//点中阳光球的操作
void collectSunshine(ExMessage* msg) {
	int count = ballMax;
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			//以下代码已被优化
			//int x = balls[i].x;
			//int y = balls[i].y;
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;

			if (msg->x > x && msg->x<x + w &&
				msg->y>y && msg->y < y + h) {
				//阳光消失，被之后的阳光移动的操作取代
				//balls[i].used = 0;
				balls[i].status = SUNSHINE_COLLECT;
				//以下代码已被优化
				//sunShine += 25;
				//mciSendString("play res/sound/sunshine.mp3", 0, 0, 0);
				//XYchange(x, y, &balls[i].xoff, &balls[i].yoff);
				PlaySound("res/sound/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);//以异步方式进行播放
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2{150, 0};
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);//c++运算符重载
				float off = 10;
				balls[i].speed = 1.0 / (distance / off);//1单位能换成几段
				break;
			}
		}
	}
}

void createZM() {
	if (zmCount > ZM_MAX) {
		return;
	}

	static int count = 0;//僵尸产生计数，不能太频繁
	static int zmFre = 70;//第一个僵尸出现的时间
	count++;
	if (count > zmFre) {
		count = 0;
		zmFre = rand() % 20 + 200;//时间范围是200-219

		int i;
		if (zms[0].dead == 1) {
			for (i = 1; i < zmMax && zms[i].used; i++);
		}
		else {
			for (i = 0; i < zmMax && zms[i].used; i++);
		}
		if (i >= zmMax) return;
		//如果这个时候的僵尸数量已经足够直接退出
		if (i < zmMax) {
			memset(&zms[i], 0, sizeof(zms[0]));
			//更新数据
			zms[i].used = 1;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 6;//0..5;
			zms[i].y = 60 + zms[i].row * MAP_HEIGHT;
			if (i == 0)
				zms[i].speed = 1;
			zms[i].speed = 2;
			zms[i].blood = 200;
			//因为第一个僵尸是举旗僵尸，死了以后不会再次出现
			//所以zms[0].dead不会更新
			if(i!=0)
				zms[i].dead = 0;//不初始化则不更新僵尸
			zmCount++;
		}
	}	
}

void updateZM() {
	static int count = 0;
	count++;
	if (count > 1) {
		count = 0;
		//更新僵尸的位置
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				//僵尸已经接近房子
				if (zms[i].x < 0) {
					//printf("GAME OVER\n");
					//MessageBox(NULL, "over", "over", 0);//待优化
					//exit(0);//待优化
					gameStatus = FAIL;
				}
			}
		}

		static int zmFirst = 1;
		//一个标记量，判断当前僵尸是否是第一个僵尸，渲染为举旗僵尸
		if (zmFirst && zms[0].x == 880) {
			zmFirst = 0;
			//该if功能仅播放第一个僵尸出现的音效
			mciSendString("play res/sound/awooga.mp3", 0, 0, 0);
		}
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					//如果僵尸死亡
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 12) {
						zms[i].used = 0;
						killCount++;
						if (killCount == ZM_MAX) {
							gameStatus = WIN;
						}
					}
				}
				//else if (zms[i].eating) {
				//	zms[i].frameIndex++;
				//	int zmType = PU_TONG;
				//	int index = zms[i].frameIndex;
				//	if (i == 0)
				//		zmType = JU_QI;
				//	if (imgZM[zmType][index] == NULL)
				//		zms[i].frameIndex = 0;
				//}
				else {
					zms[i].frameIndex++;
					int zmType = PU_TONG;
					int index = zms[i].frameIndex;
					if (i == 0)
						zmType = JU_QI;
					if (imgZM[zmType][index] == NULL)
						zms[i].frameIndex = 0;
				}
			}
		}
	}
}

void drawZM() {
	//绘制僵尸
	IMAGE* img = NULL;
	int count = zmMax;
	for (int i = 0; i < count; i++) {
		int index = zms[i].frameIndex;
		int zmType = PU_TONG;
		if (zms[i].used && zms[i].dead) {
			//僵尸已死亡
			IMAGE img1 = imgZMDead[index];
			IMAGE img2 = imgZMD_Head[index];
			putimagePNG(zms[i].x, zms[i].y - (MAP_HEIGHT / 2), &img1);
			putimagePNG(zms[i].x + 50, zms[i].y - (MAP_HEIGHT / 2), &img2);
		}
		else if (zms[i].used && zms[i].eating) {
			//僵尸吃植物
			if (i == 0) {
				zmType = JU_QI;
				img = imgZMEat[zmType][index];
			}
			else {
				img = imgZMEat[zmType][index];
			}
			putimagePNG(zms[i].x, zms[i].y - (MAP_HEIGHT / 2), img);
		}
		else if (zms[i].used && zms[i].dead == 0) {
			//僵尸正常行走过程
			if (i == 0) {
				zmType = JU_QI;
				img = imgZM[zmType][index];
			}
			else {
				img = imgZM[zmType][index];
			}
			putimagePNG(zms[i].x, zms[i].y - (MAP_HEIGHT / 2), img);
		}
	}
}

void shoot() {
	int lines[5] = { 0 };//初始化行标记
	int zmCount = sizeof(zms) / sizeof(zms[0]);//僵尸数量
	int dangerX = 800; //僵尸出现后子弹开始发射的目标位置
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;
		}
	}//设置僵尸的行号 

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == WAN_DOU + 1 && lines[i] == 1) {
				//判断是否种了豌豆
				static int count = 0;//计数器
				count++;
				if(count >= 31) {
					count = 0;

					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k < bulletMax && bullets[k].used == 0) {
						bullets[k].used = 1;
						bullets[k].row = i;
						bullets[k].speed = 18;

						bullets[k].blast = 0;
						bullets[k].frameIndex = 0;

						int zwX = 140 + j * MAP_WIDTH + 10;
						int zwY = ZHI_WU_HEIGHT + i * MAP_HEIGHT + 10;
						bullets[k].x = zwX + (imgZhiWu[map[i][j].type - 1][0]->getwidth() / 2);
						bullets[k].y = zwY;
					}//改变子弹的数据
				}
			}
		}
	}
}


void updateBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
	 		if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = 0;
			}

			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex >= 4) {
					bullets[i].used = 0;
				}
			}
		}
	}
}

//碰撞检测
void collisionCheck() {
	checkBullet2ZM();//子弹和僵尸的碰撞检测
	checkZm2ZhiWu();//僵尸对植物的碰撞检测
}

void checkBullet2ZM() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);//子弹数量
	int zCount = sizeof(zms) / sizeof(zms[0]);//僵尸数量
	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == 0 || bullets[i].blast)continue;

		for (int k = 0; k < zCount; k++) {
			if (zms[k].used == 0)continue;

			//x1、x2是子弹爆炸的范围
			int x1 = zms[k].x + 60;
			int x2 = zms[k].x + 110;
			int x = bullets[i].x;//子弹当前的坐标
			if (zms[k].dead == 0 && bullets[i].row == zms[k].row && x > x1 && x < x2) {
				mciSendString("play res/sound/kernelpult.mp3", 0, 0, 0);
				//PlaySound("res/sound/kernelpult.wav", NULL, SND_FILENAME | SND_ASYNC);
				//判定发生碰撞
				zms[k].blood -= 20;
				bullets[i].blast = 1;
				bullets[i].speed = 0;

				if (zms[k].blood <= 0) {
					zms[k].dead = 1;
					zms[k].speed = 0;
					zms[k].frameIndex = 0;
				}
				//已经和僵尸匹配碰撞，无需和其他的僵尸检查
				break;
			}
		}
	}
}

void checkZm2ZhiWu() {
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zCount; i++) {
		if (zms[i].dead) continue;

		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0) continue;

			//      x3    
			//      [ 僵尸 ]
			// [ 植物 ]
			// x1     x2
			int zhiwuX = 140 + k * MAP_WIDTH;
			int x1 = zhiwuX + 10;
			int x2 = zhiwuX + 60;
			int x3 = zms[i].x + 60;
			if (x3 > x1 && x3 < x2) {
				//达到吃植物的条件
				if (map[row][k].catched) {
					//植物被选中
					map[row][k].deadTime++;
					if (map[row][k].deadTime>=120) {
						map[row][k].catched = 0;
						map[row][k].deadTime = 0;
						map[row][k].type = 0;
						zms[i].eating = 0;
						zms[i].frameIndex = 0;
						zms[i].speed = 1;
					}
					else if (zms[i].dead == 1) {
						map[i][k].catched = 0;
					}
				 }
				else {
					map[row][k].catched = 1;
					map[row][k].deadTime = 0;
					zms[i].eating = 1;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}