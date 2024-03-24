#include <stdio.h>
#include <graphics.h>//��װeasyxͼ��⣬easyxͼ�ο��ͷ�ļ�
#include <time.h>
#include <math.h>
#include "tools.h"
#include "vector2.h"//�������˶��Ĺ���
#include <mmsystem.h>//���ֿ�
#pragma comment(lib,"winmm.lib")//������������winmm.lib���ļ����ӵ�������

/*
*���ݺ궨��
*/
#define WIN_WIDTH 900//���ڵĸ߶ȡ����
#define WIN_HEIGHT 600
#define ZHI_WU_WIDTH 52 //ֲ�￨�ƵĿ��
#define ZHI_WU_HEIGHT 52*1.28 //����=1.28
#define MAP_WIDTH 81 //ֲ����ֲ�ĵ�ͼ���о���о�
#define MAP_HEIGHT 102
#define ZM_MAX 10
/*
*ö�ٴ洢��Ϸ״̬��ÿһ��������������Ϸ�ĵ�ǰ״̬
*/
enum { GOING, WIN, FAIL };
int killCount;//�Ѿ�ɱ���Ľ�ʬ����
int zmCount;//�Ѿ����ֵĽ�ʬ����
int gameStatus;//��Ϸ��ǰ״̬

/*
*ö�ٴ洢ֲ�￨�ƣ����һ��ZHI_WU_COUNT����������
*/
enum ZW { WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT };

/*
* ��������
*/
IMAGE imgBg;//��ʾ����ͼƬ
IMAGE imgBar;//��ʾֲ��ѡ����
IMAGE imgCards[ZHI_WU_COUNT];//ֲ�￨�Ƶ�����
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];//ֲ���֡��ͼ

/*
*����ֲ��ؿ���������
*/
struct zhiwu {
	int type;		//0��û��ֲ�1����һ��ֲ��
	int frameIndex;//����֡���±�

	int catched;//�Ƿ񱻽�ʬ����
	int deadTime;//���������� ʵ�ֽ�Ϊ��

	int timer;//�����ʱ��
	int x, y;
};
//�ýṹ����ĳ���ؿ��Ƿ���ֲֲ��
struct zhiwu map[5][9];//ֲ���ͼ

int curX, curY;//��ǰѡ�е�ֲ�����ƶ������е�����
int curZhiWu;//��ѡѡ�е�ֲ�0��ʾû��ѡ��

/*
*ö�ٴ洢���⣬���һ��ZHI_WU_COUNT����������
*/
enum SUNSHINE { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };

/*
*����������������
*/
struct sunshineBall {

	int frameIndex;//��ǰ��ͼƬ֡�����
	int used;//�Ƿ���ʹ��
	int mytimer;//��ʱ��

	//���´����ѱ��Ż�
	int x, y;//��������Ʈ������е�����λ�ã�x���䣩
	int dextY;//Ʈ���Ŀ��λ�õ�����
	float xoff;
	float yoff;//xy���ƫ����

	float t;//���������ߵ�ʱ���0..1
	vector2 p1, p2, p3,p4;//p1��p4�ֱ��������յ㣻p2��p3���������Ƶ�
	vector2 pCur;//��ǰʱ���������λ��
	float speed;//��������ٶ�
	int status;//�������״̬
};
struct sunshineBall balls[10];//�ڴ��
IMAGE imgSunshineBall[29];//����֡��ͼ����
int ballMax = sizeof(balls) / sizeof(balls[0]);//������������
int sunShine;//�����ռ���

/*
*ö�ٴ洢��ʬ���࣬���һ��JIANG_SHI_COUNT����������
*/
enum ZM{ JU_QI, PU_TONG, JIANG_SHI_COUNT};

/*
*������ʬ��������
*/
struct zm {
	int x, y;
	int frameIndex;//֡�±�
	int used;
	int speed;
	int row;//��ʬ����һ��
	int blood;
	int dead;
	int eating;//���ڳ�ֲ���״̬
};
struct zm zms[10];//��ʬ����
IMAGE* imgZM[JIANG_SHI_COUNT][20];//��ʬ��֡��ͼ
int zmMax = sizeof(zms) / sizeof(zms[0]);//��ʬ���������
IMAGE imgZMDead[12];//��ʬ����ͼƬ
IMAGE imgZMD_Head[12];//��ʬ����ͷͼƬ
IMAGE* imgZMEat[JIANG_SHI_COUNT][21];//��ʬ�Ե�֡ͼƬ
IMAGE imgZMStand[14];//��ʬվ����ͼƬ

/*
*�ӵ�����������
*/
struct bullet
{
	int x, y;
	int row;//��
	int used;
	int speed;
	int blast;//�Ƿ����н�ʬ
	int frameIndex;//֡���
};
struct bullet bullets[30];//�ӵ��ڴ��
IMAGE imgBulletNormal;//�����ӵ���ͼƬ
IMAGE imgBullBlast[4];//�ӵ���ײ��ͼƬ
int bulletMax = sizeof(bullets) / sizeof(bullets[0]);//�ӵ����������

/*
* ��������
*/
void gameInit();/*��Ϸ��ʼ��*/
void updateWindow();/*���´���*/
void userClick();/*�û��ĵ������*/
void updateGame();/*������Ϸ����*/
void startUI();/*�����˵�*/
void createSunshine();/*��������*/
void updateSunshine();/*����״̬����*/
void collectSunshine(ExMessage*);/*�����ռ�����*/
void XYchange(int, int, float*, float*);//ƫ�������㺯��
void createZM();/*������ʬ*/
void updateZM();/*��ʬ״̬����*/
void drawZM();/*���ƽ�ʬ*/
void shoot();//�����㶹�ӵ�
void updateBullets();//�����ӵ�
void collisionCheck();//ʵ����ײ���
void checkBullet2ZM();/*ʵ���㶹�ӵ��ͽ�ʬ����ײ���*/
void checkZm2ZhiWu();/*ʵ�ֽ�ʬ��ֲ��ļ��*/
void drawsunshine();/*��������*/
void updateZhiWu();/*����ֲ�������*/
void drawCards();/*��Ⱦ��Ƭ*/
void drawmap();/*��Ⱦֲ�￨Ƭ��ֲ���*/
void drawBullets();/*��Ⱦ�ӵ�*/
void viewScence();//ת��
void barsDown();//���������½���Ч
int checkOver();//�����Ϸ״̬

int main(void) {
	gameInit(); //��Ϸ���ݼ��غ���
	startUI();//�˵����غ���
	viewScence();//ת��
	barsDown();//���������½���Ч

	int timer = 0;//������
	int flag = 1;//�Ƿ�ˢ����Ϸ���ݵı�־
	while (1) {
		userClick();//����¼�
		timer += getDelay();//��ȡһ����ʱʱ��
		if (timer > 70) {
			flag = 1;
			timer = 0;
		}
		if (flag) {
			flag = 0;
			updateWindow();
			updateGame();
			checkOver();//�����Ϸ״̬
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
	//������1398*600,֮ǰ��ʼ��ʱ���й�����
	int xMin = WIN_WIDTH - imgBg.getwidth();//900-1398
	vector2 points[9] = {
		//��Ϊ������С��ֱ������λ��
		{550, 80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340} };
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 14;
	}
		
	int count = 0;
	//Ƭ��Ѳ��
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

	//ͣ��2s����
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

	//�л���Ϸ����
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
	//������Ϸ����ͼƬ,,�ߣ���=2.33
	//���loadimage�������ַ�����Ϊ�����ֽ��ַ����� ����->�߼�
	int Bg_height = WIN_HEIGHT;
	loadimage(&imgBg, "res/Bg.jpg", Bg_height * 2.33, Bg_height);
	//��ֲ�ﲥ��ͼƬ��������ڴ�����Ϊ0
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));//��ͼ��ʼ��

	//��Ϸ״̬�ĳ�ʼ��
	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;

	//��ʼ��ֲ�￨��,ֲ��ѡ�����ߣ���=6.10
	int Bar_height = 80;
	loadimage(&imgBar, "res/Bar.png", Bar_height * 6.10, Bar_height);
	char name[64];//����ͼƬ��·��
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name, ZHI_WU_WIDTH, ZHI_WU_HEIGHT);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//���ж�ͼƬ�ļ��Ƿ���ڣ��ټ���
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else break;
		}
	}

	curZhiWu = 0;//ѡ���ֲ������
	sunShine = 125;//��ʼ������

	//��ʼ������ͼƬ����
	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}
	//�����������
	srand(time(NULL));
	//������Ϸ��ͼ�񴰿�
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//��������������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 25;
	f.lfWidth = 10;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//�����Ч��
	settextstyle(&f);//�����������
	setbkmode(TRANSPARENT);//����͸��
	setcolor(BLACK);//������ɫ

	//��ʼ����ʬ
	memset(imgZM, 0, sizeof(imgZM));//�ѽ�ʬ����ͼƬ��������ڴ�����Ϊ0
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < JIANG_SHI_COUNT; i++) {
		for (int j = 0; j < 20; j++) {
			//���ɽ�ʬ֡����ͼƬ��
			sprintf_s(name, sizeof(name), "res/zm/%d/%d.png", i, j + 1);
			//���ж�ͼƬ�ļ��Ƿ���ڣ��ټ���
			if (fileExist(name)) {
				imgZM[i][j] = new IMAGE;
				loadimage(imgZM[i][j], name);
			}
			else break;
		}
	}

	//�ӵ�
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//��ʼ���㶹�ӵ���֡ͼƬ����
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png",
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k, true);
	}

	//��ʬ�����ĳ�ʼ��
	for (int i = 0; i < 12; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
		sprintf_s(name, sizeof(name), "res/zm_dead_head/%d.png", i + 1);
		loadimage(&imgZMD_Head[i], name);
	}

	//��ʬ�Զ����ĳ�ʼ��
	memset(imgZMEat, 0, sizeof(imgZMEat));//�ѽ�ʬͼƬ��������ڴ�����Ϊ0
	for (int i = 0; i < JIANG_SHI_COUNT; i++) {
		for (int j = 0; j < 21; j++) {
			//���ɽ�ʬ֡����ͼƬ��
			sprintf_s(name, sizeof(name), "res/zm_eat/%d/%d.png", i, j + 1);
			//���ж�ͼƬ�ļ��Ƿ���ڣ��ټ���
			if (fileExist(name)) {
				imgZMEat[i][j] = new IMAGE;
				loadimage(imgZMEat[i][j], name);
			}
			else break;
		}
	}
	//��ʬվ���ĳ�ʼ��
	for (int i = 0; i < 14; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZMStand[i], name);
	}
}


void updateWindow() {
	BeginBatchDraw();//easyx�еĺ�������ʼ����

	//��Ⱦ��Ϸ����
	putimage(-112, 0, &imgBg);
	//putimage(250, 0, &imgBar);�ñ߿������ý�����ÿ�
	putimagePNG(150, 0, &imgBar);
	drawCards();

	//��ȾĿǰֲ����ֲ���
	drawmap();

	//��Ⱦ�������
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);
	outtextxy(175, 55, scoreText);//�������

	//��Ⱦ��ʬ
	drawZM();

	//��Ⱦ�ӵ�ͼƬ
	drawBullets();

	//��Ⱦ�϶������е�ֲ��
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		//curX-img->getwidth()/2�ù����ͼƬ������
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	//��Ⱦ����
	drawsunshine();

	EndBatchDraw();//easyx�еĺ���������˫����
}

void drawBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {
				//�ӵ����е���Ⱦ
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
				//���´����ѱ��Ż�
				//int x = 250 + j * MAP_WIDTH + 10;
				//int y = ZHI_WU_HEIGHT + i * MAP_HEIGHT + 10;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				//���´����ѱ��Ż�
				//putimagePNG(x, y, imgZhiWu[ZhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[ZhiWuType][index]);
			}
		}
	}
}

void drawCards() {
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//��һ�ſ��ƾ��봰����߽�ľ���Ϊ70
		int x = 220 + i * ZHI_WU_WIDTH;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

void drawsunshine() {
	for (int i = 0; i < ballMax; i++) {
		//���´����ѱ��Ż�
		//if (balls[i].used || balls[i].xoff != 0) {
		if (balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			//���´����ѱ��Ż�
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}
}

void userClick() {
	ExMessage msg;//��Ϣ����
	static int status = 0;//��¼״̬

	if (peekmessage(&msg)) {
		//�жϵ�ǰ�����������Ϣ
		if (msg.message == WM_LBUTTONDOWN) {
			curX = msg.x;
			curY = msg.y;
			//��갴��
			if (msg.x > 220 && msg.x < 220 + ZHI_WU_WIDTH * ZHI_WU_COUNT && msg.y < ZHI_WU_HEIGHT) {
				//���ѡ��Χ��ֲ�￨�ƹ�������Χ��,index�պ����±�
				int index = (msg.x - 220) / ZHI_WU_WIDTH;
				if (index == WAN_DOU && sunShine < 100) return;
				if (index == XIANG_RI_KUI && sunShine < 50) return;
				status = 1;
				curZhiWu = index + 1;
			}
			else {//�ռ�����
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			//����ƶ�
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {
			//���ſ�
			//ֲ����ֲ�ؿ��102 ��81
			if (curZhiWu == 1) sunShine -= 100;
			if (curZhiWu == 2) sunShine -= 50;

			if (msg.x > 140 && msg.y > ZHI_WU_HEIGHT && msg.y < WIN_HEIGHT - 10) {
				int row = (msg.y - ZHI_WU_HEIGHT) / MAP_HEIGHT;
				int col = (msg.x - 140) / MAP_WIDTH;
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
					//���´����ѱ��Ż�
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
	//����ֲ�����ֲ���
	updateZhiWu();

	createSunshine();//��������
	updateSunshine();//��������״̬

	createZM();//������ʬ
	updateZM();//���½�ʬ״̬

	shoot();//�����㶹�ӵ�
	updateBullets();//�����ӵ�

	collisionCheck();//ʵ���㶹�ӵ��ͽ�ʬ����ײ���
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
	int flag = 0;//��ť�Ƿ�ѡ��
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
		EndBatchDraw();

		ExMessage msg;//��ť״̬
		if (peekmessage(&msg)) {
			//�����ð�ť����
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 475 && msg.x < 475 + 300 &&
				msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
				putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
			}
			//�Ѿ����й���ť����ʱ�ַſ�
			else if (msg.message == WM_LBUTTONUP && flag) {
				flag = 0;
				putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
				int timer = 0;
				while (timer < 100) {
					timer += getDelay();//��ȡһ����ʱʱ��
				}
				return;
			}
		}
	}
}

void createSunshine() {
	static int count = 0;//�����������������̫Ƶ��
	static int fre = 50;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 200;//100-299
		count = 0;

		//���������ȡһ������ʹ�õ�
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		//������������
		balls[i].used = 1;
		balls[i].frameIndex = 0;
		balls[i].mytimer = 0;

		//���´����ѱ��Ż�
		//balls[i].x = 250 + rand() % (800 - 250);//����x�ķ�Χ��250-800
		//balls[i].y = 0;
		//balls[i].dextY = 60 + (rand() % 5) * MAP_HEIGHT;//����y�ķ�Χ��60-600
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;//xy���ƫ����

		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(140 + rand() % (800 - 140), 0);
		balls[i].p4 = vector2(balls[i].p1.x, 60 + (rand() % 5) * MAP_HEIGHT);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}

	//���տ���������
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
					balls[k].speed = 0.10;//10֡����Ŀ�ĵ�
					balls[k].t = 0;
				}
			}
		}
	}
}

void updateSunshine() {
	for (int i = 0; i < ballMax; i++) {
		//�����������Ѿ���ʹ��
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
			//	//������ڵ�ʱ�䳬���涨ʱ��
			//	if(balls[i].mytimer>100)
			//		balls[i].used = 0;
			//}
		}
		//else if (balls[i].xoff != 0) {
		//	//ƫ������Ϊ�Ǹ��������ܲ�׼ȷ ���ÿ�ζ����м���
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
	//����ƫ����
	float destY = 5;
	float destX = 260;
	float angle = atan((y - destY) / (x - destX));
	*n = 6 * cos(angle);
	*m = 6 * sin(angle);
}

//����������Ĳ���
void collectSunshine(ExMessage* msg) {
	int count = ballMax;
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			//���´����ѱ��Ż�
			//int x = balls[i].x;
			//int y = balls[i].y;
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;

			if (msg->x > x && msg->x<x + w &&
				msg->y>y && msg->y < y + h) {
				//������ʧ����֮��������ƶ��Ĳ���ȡ��
				//balls[i].used = 0;
				balls[i].status = SUNSHINE_COLLECT;
				//���´����ѱ��Ż�
				//sunShine += 25;
				//mciSendString("play res/sound/sunshine.mp3", 0, 0, 0);
				//XYchange(x, y, &balls[i].xoff, &balls[i].yoff);
				PlaySound("res/sound/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);//���첽��ʽ���в���
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2{150, 0};
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);//c++���������
				float off = 10;
				balls[i].speed = 1.0 / (distance / off);//1��λ�ܻ��ɼ���
				break;
			}
		}
	}
}

void createZM() {
	if (zmCount > ZM_MAX) {
		return;
	}

	static int count = 0;//��ʬ��������������̫Ƶ��
	static int zmFre = 70;//��һ����ʬ���ֵ�ʱ��
	count++;
	if (count > zmFre) {
		count = 0;
		zmFre = rand() % 20 + 200;//ʱ�䷶Χ��200-219

		int i;
		if (zms[0].dead == 1) {
			for (i = 1; i < zmMax && zms[i].used; i++);
		}
		else {
			for (i = 0; i < zmMax && zms[i].used; i++);
		}
		if (i >= zmMax) return;
		//������ʱ��Ľ�ʬ�����Ѿ��㹻ֱ���˳�
		if (i < zmMax) {
			memset(&zms[i], 0, sizeof(zms[0]));
			//��������
			zms[i].used = 1;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 6;//0..5;
			zms[i].y = 60 + zms[i].row * MAP_HEIGHT;
			if (i == 0)
				zms[i].speed = 1;
			zms[i].speed = 2;
			zms[i].blood = 200;
			//��Ϊ��һ����ʬ�Ǿ��콩ʬ�������Ժ󲻻��ٴγ���
			//����zms[0].dead�������
			if(i!=0)
				zms[i].dead = 0;//����ʼ���򲻸��½�ʬ
			zmCount++;
		}
	}	
}

void updateZM() {
	static int count = 0;
	count++;
	if (count > 1) {
		count = 0;
		//���½�ʬ��λ��
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				//��ʬ�Ѿ��ӽ�����
				if (zms[i].x < 0) {
					//printf("GAME OVER\n");
					//MessageBox(NULL, "over", "over", 0);//���Ż�
					//exit(0);//���Ż�
					gameStatus = FAIL;
				}
			}
		}

		static int zmFirst = 1;
		//һ����������жϵ�ǰ��ʬ�Ƿ��ǵ�һ����ʬ����ȾΪ���콩ʬ
		if (zmFirst && zms[0].x == 880) {
			zmFirst = 0;
			//��if���ܽ����ŵ�һ����ʬ���ֵ���Ч
			mciSendString("play res/sound/awooga.mp3", 0, 0, 0);
		}
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					//�����ʬ����
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
	//���ƽ�ʬ
	IMAGE* img = NULL;
	int count = zmMax;
	for (int i = 0; i < count; i++) {
		int index = zms[i].frameIndex;
		int zmType = PU_TONG;
		if (zms[i].used && zms[i].dead) {
			//��ʬ������
			IMAGE img1 = imgZMDead[index];
			IMAGE img2 = imgZMD_Head[index];
			putimagePNG(zms[i].x, zms[i].y - (MAP_HEIGHT / 2), &img1);
			putimagePNG(zms[i].x + 50, zms[i].y - (MAP_HEIGHT / 2), &img2);
		}
		else if (zms[i].used && zms[i].eating) {
			//��ʬ��ֲ��
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
			//��ʬ�������߹���
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
	int lines[5] = { 0 };//��ʼ���б��
	int zmCount = sizeof(zms) / sizeof(zms[0]);//��ʬ����
	int dangerX = 800; //��ʬ���ֺ��ӵ���ʼ�����Ŀ��λ��
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;
		}
	}//���ý�ʬ���к� 

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == WAN_DOU + 1 && lines[i] == 1) {
				//�ж��Ƿ������㶹
				static int count = 0;//������
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
					}//�ı��ӵ�������
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

//��ײ���
void collisionCheck() {
	checkBullet2ZM();//�ӵ��ͽ�ʬ����ײ���
	checkZm2ZhiWu();//��ʬ��ֲ�����ײ���
}

void checkBullet2ZM() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);//�ӵ�����
	int zCount = sizeof(zms) / sizeof(zms[0]);//��ʬ����
	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == 0 || bullets[i].blast)continue;

		for (int k = 0; k < zCount; k++) {
			if (zms[k].used == 0)continue;

			//x1��x2���ӵ���ը�ķ�Χ
			int x1 = zms[k].x + 60;
			int x2 = zms[k].x + 110;
			int x = bullets[i].x;//�ӵ���ǰ������
			if (zms[k].dead == 0 && bullets[i].row == zms[k].row && x > x1 && x < x2) {
				mciSendString("play res/sound/kernelpult.mp3", 0, 0, 0);
				//PlaySound("res/sound/kernelpult.wav", NULL, SND_FILENAME | SND_ASYNC);
				//�ж�������ײ
				zms[k].blood -= 20;
				bullets[i].blast = 1;
				bullets[i].speed = 0;

				if (zms[k].blood <= 0) {
					zms[k].dead = 1;
					zms[k].speed = 0;
					zms[k].frameIndex = 0;
				}
				//�Ѿ��ͽ�ʬƥ����ײ������������Ľ�ʬ���
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
			//      [ ��ʬ ]
			// [ ֲ�� ]
			// x1     x2
			int zhiwuX = 140 + k * MAP_WIDTH;
			int x1 = zhiwuX + 10;
			int x2 = zhiwuX + 60;
			int x3 = zms[i].x + 60;
			if (x3 > x1 && x3 < x2) {
				//�ﵽ��ֲ�������
				if (map[row][k].catched) {
					//ֲ�ﱻѡ��
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