#include "Arduino.h"
#include "lvgl.h"
#include "TFT_eSPI.h"


int AprevTime = 0;
int Anim = 0;
#define key1_pin 10
#define key2_pin 6

#define led1_pin 0
#define led2_pin 1
unsigned char key1_sta = 0;
unsigned char key2_sta = 0;
char t_ht=0,t_horse=0,sta=1;
unsigned char t1=0,t2=0,t0=1,t=1;
char t_scanf=1;
char tt=0;

unsigned char key_text=0;

void exti_init(void);
void key1_isr(void);
void key2_isr(void);
void imgAnim(void);


//端口初始化
void exti_init(void) {
  pinMode(key1_pin, INPUT_PULLUP);  //设置引脚为输入上拉模式
  pinMode(key2_pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(key1_pin), key1_isr, FALLING);  //设置下降沿触发
  attachInterrupt(digitalPinToInterrupt(key2_pin), key2_isr, FALLING);
}

LV_IMG_DECLARE(scan_a);  //horse_a planet_a  earth  ht_a
LV_IMG_DECLARE(horse_b);
LV_IMG_DECLARE(ht_b);

static const uint16_t screenWidth = 170;
static const uint16_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenHeight * screenWidth / 10];

TFT_eSPI tft = TFT_eSPI(170, 320);

#define LV_DELAY(x) \
  do { \
    uint32_t t = x; \
    while (t--) { \
      lv_timer_handler(); \
      delay(1); \
    } \
  } while (0);


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}


static lv_obj_t *logo_imga = NULL;
static lv_obj_t *logo_imgb = NULL;
static lv_obj_t *logo_imgc = NULL;


void setup(){
  
  lv_init();
  tft.begin();
  tft.setRotation(1);

  // Serial.begin(115200);
  pinMode(led1_pin, OUTPUT);  //设置引脚为输出模式
  pinMode(led2_pin, OUTPUT);

  exti_init();
  digitalWrite(led1_pin, key1_sta);
  digitalWrite(led2_pin, key2_sta);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenHeight * screenWidth / 10);


  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 170;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;  
  lv_disp_drv_register(&disp_drv);

  //设置黑色的背景
  static lv_style_t style;  
  lv_style_init(&style);
  lv_style_set_bg_color(&style, lv_color_white());
  lv_obj_add_style(lv_scr_act(), &style, 0);


  LV_IMG_DECLARE(spotpear);
  lv_obj_t *logo_img = lv_gif_create(lv_scr_act());
  lv_obj_center(logo_img);
  lv_obj_align(logo_img, LV_ALIGN_LEFT_MID, 0, 0);
  lv_gif_set_src(logo_img, &spotpear);
  LV_DELAY(250);
  lv_obj_del(logo_img);

  static lv_style_t style1;  
  lv_style_init(&style1);
  lv_style_set_bg_color(&style1, lv_color_black());
  lv_obj_add_style(lv_scr_act(), &style1, 0);
}

void loop() {
    lv_timer_handler();
    delay(1);
    if(t_ht==1 || t_scanf==1 || t_horse==1)
    {
    t_horse=0;
    t_scanf=0;
    t_ht=0;
    }

    else if(key_text==0)
    {
    logo_imga = lv_gif_create(lv_scr_act());
    lv_obj_center(logo_imga);
    lv_gif_set_src(logo_imga, &scan_a);
    key_text=4;
    }

    else if(key_text==1)
    {
    if(t2==1 ){
        lv_obj_del(logo_imgc);
        logo_imgb = lv_gif_create(lv_scr_act());
        lv_obj_center(logo_imgb);
        lv_gif_set_src(logo_imgb, &ht_b);
        t2=0;
      }
      if(t0==1){
      lv_obj_del(logo_imga);
      logo_imgb = lv_gif_create(lv_scr_act());
      lv_obj_center(logo_imgb);
      lv_gif_set_src(logo_imgb, &ht_b);
      t0=0;
      }
      if(t==0){
      lv_obj_center(logo_imgb);
      lv_gif_set_src(logo_imgb, &ht_b);
      t=0;
      }
      t1=1;
      key_text=4;
    }

    else if(key_text==2)
    {
     if(t1==1 ){
        lv_obj_del(logo_imgb);
        logo_imgc = lv_gif_create(lv_scr_act());
        lv_obj_center(logo_imgc);
        lv_gif_set_src(logo_imgc, &horse_b);
        t1=0;
      }
      if(t0==1){
      lv_obj_del(logo_imga);
      logo_imgc = lv_gif_create(lv_scr_act());
      lv_obj_center(logo_imgc);
      lv_gif_set_src(logo_imgc, &horse_b);
      t0=0;
      }
      t2=1;
      key_text=4;
    }
}


void key1_isr(void) {
  key1_sta = !key1_sta;
  key_text = 1;
  t_ht=1;
  
}
void key2_isr(void) {
  key2_sta = !key2_sta;
  key_text = 2;
  t_horse=1;
}


