#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "thread_pool.h"
//#include "font.h"

#define LCD_WIDTH 800
#define LCD_HEIGHT 480

#define LCD_PATH "/dev/fb0"
#define TOUCH_SCREENT_PATH "/dev/input/event0"

#define RED "\e[0;31m"
#define BLUE "\e[0;34m"
#define NONE "\e[0m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33;1m"
#define PURPLE "\e[0;35m"
#define GR "\e[0;36m"

/**
 * 坐标结构体
*/
typedef struct index_x_y
{
  int x; //横坐标
  int y; //纵坐标
} xy;

/**
 * 参数
*/
struct arg
{
  int arg1;
  char arg2[256];
  char arg3[512];
  char *arg4[10];
  void *ptr[10];
};

/**
 * 消息队列 数据
*/
struct msgbuf
{
  long int mtype; /*消息的类型 */
  int flag;       /*播放状态*/
  int pos;        /*播放进度 1~100*/
  int time;       /*总时长*/
  char mtext[50]; /*备注*/
};

/**
 * 共享内存 数据
*/
typedef struct shmbuf
{
  float pos;  //进度值
  int volume; //声音值
  int pause;  //暂停标志位
  int time;   //总时长
} shmbuf;

/****************touch_screen.c start*****************************************************/

/**
 * 获取坐标
*/
int get_xy(xy *p);

/**
 * 获取坐标 plus
*/
bool get_xy_plus(xy *p);

/**
 * 是否在触摸键区域
 * (x1,y1),(x2,y2)
 * p: 坐标结构体指针;
*/
bool is_key_area(xy *p, int x1, int y1, int x2, int y2);

/****************touch_screen.c end *****************************************************/

/****************font.c start *****************************************************/

/**
 * 在屏幕上自定义显示文字
*/
// void fonts(char *text,
//            unsigned int x,
//            unsigned int y,
//            int size,
//            unsigned int color,
//            int Xbg,
//            int Ybg,
//            int width,
//            int height,
//            unsigned long bgc);
/****************font.c end *****************************************************/

/****************tool.c start *****************************************************/

/**
 * 检查指定进程是否运行
 * return : 成功 返回pid 失败返回 -1
*/
int get_pid(char *name);

/**
 * 时间格式化
 * flag: ture 时分秒 false 分秒
*/
void time_format(int second, char *time_str, bool flag);

int Init_Font(void);                 //显示字体库前先调用本函数进行初始化
void UnInit_Font(void);              //程序退出前，调用本函数
int Clean_Area(int X,                //x坐标起始点
               int Y,                //y坐标起始点
               int width,            //绘制的宽度
               int height,           //绘制的高度
               unsigned long color); //往屏幕指定区域填充颜色

int Display_characterX(unsigned int x,        //x坐标起始点
                       unsigned int y,        //y坐标起始点
                       unsigned char *string, //GB2312 中文字符串
                       unsigned int color,    //字体颜色值
                       int size);             //字体放大倍数 1~8

/**
 * 打印图片到屏幕
 * filename : 图片路径
 * setx,sety:要显示图片的起始位置
 * clean:是否清屏
*/
int display_picture(char *filename, int setx, int sety, bool clean);

/**
 * 圆形图片
*/
int display_picture_circle(char *filename, int setx, int sety, bool clean);

/****************tool.c end *****************************************************/

#endif