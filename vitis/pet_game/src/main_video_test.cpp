/*****************************************************************/ /**
                                                                     * @file main_video_test.cpp
                                                                     *
                                                                     * @brief Basic test of 4 basic i/o cores
                                                                     *
                                                                     * @author p chu
                                                                     * @version v1.0: initial release
                                                                     *********************************************************************/

// #define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"

void test_start(GpoCore *led_p)
{
   int i;

   for (i = 0; i < 20; i++)
   {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p)
{
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p)
{
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 */
void osd_check(OsdCore *osd_p)
{
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++)
   {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p)
{
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++)
   {
      frame_p->clr_screen(0x008); // dark green
      for (int j = 0; j < 20; j++)
      {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
      sleep_ms(300);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p)
{
   int x, y;

   mouse_p->bypass(0);
   // clear top and bottom lines
   for (int i = 0; i < 32; i++)
   {
      mouse_p->wr_mem(i, 0);
      mouse_p->wr_mem(31 * 32 + i, 0);
   }

   // slowly move mouse pointer
   x = 0;
   y = 0;
   for (int i = 0; i < 80; i++)
   {
      mouse_p->move_xy(x, y);
      sleep_ms(50);
      x = x + 4;
      y = y + 3;
   }
   sleep_ms(3000);
   // load top and bottom rows
   for (int i = 0; i < 32; i++)
   {
      sleep_ms(20);
      mouse_p->wr_mem(i, 0x00f);
      mouse_p->wr_mem(31 * 32 + i, 0xf00);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p)
{
   int x, y;

   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1c); // animation; blue ghost
   x = 0;
   y = 100;
   for (int i = 0; i < 156; i++)
   {
      ghost_p->move_xy(x, y);
      sleep_ms(100);
      x = x + 4;
      if (i == 80)
      {
         // change to red ghost half way
         ghost_p->wr_ctrl(0x04);
      }
   }
   sleep_ms(3000);
}

// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

// change the scene given a scene number
// let 0 be home/default
// let 1 be feed scene
// let 2 be shower scene
// let 3 be pet scene
void change_scene(unsigned scene)
{
}

void feed(int *hunger) {
   *hunger
}
void pet(int *happiness) {

}
void shower(*int cleanliness) {
   
}

// check the keyboard for a 'f' 's'
// return 0 if 'f', return 1 if 's', return -1 if none
int keyboard_check(Ps2Core *ps2_p, int *id)
{
   char ch = 0;

   if (*id == -1)
   {
      sleep_ms(100);
      *id = ps2_p->init();
   }
   if (*id == 1)
   {
      if (ps2_p->get_kb_ch(&ch))
      {
         uart.disp("\r\nch: ");
         uart.disp(static_cast<unsigned char>(ch), 16);

         if (ch == 'f')
         {
            return 0;
         }
         else if (ch == 's')
         {
            return 1;
         }
      }
   }
   return -1;
}

bool acc_check()
{
   return 0;
}

// draw the bars (5 segments)
void draw_bars()
{
}

void check_pet_health(TimerCore *timer, GpiCore *sw, int *hunger, int *happiness, int *cleanliness)
{
   uint64_t time = timer->read_time();
   uint64_t max_time = sw->read();

   if (time == max_time)
   {
      (*hunger)--;
      (*happiness)--;
      (*cleanliness)--;

      if (*hunger == 0 || *happiness == 0 || *cleanliness == 0)
      {
         // display game over screen, end game
         // show restart button
      }
      draw_bars();
      timer->clear();
   }
}

void pet_game(TimerCore *timer, Ps2Core *ps2_p, GpiCore *sw, int *id, int *hunger, int *happiness, int *cleanliness)
{
   // game cycle:
   // decrement bars
   check_pet_health(timer, sw, hunger, happiness, cleanliness);
   int keeb = keyboard_check(ps2_p, id);
   if (keeb == -1)
   {
      // invalid
   }
   else if (keeb == 0)
   {
      change_scene(1);
      feed();
   }
   else if (keeb == 1)
   {
      change_scene(2);
      shower();
   } else if (acc_check()) 
   {
      change_scene(3);
      pet();
   }


   // accelerometer check
   // if past some threshold, pet screen
   // for each action, max out corresponding bar

   // extra:
   // achievements?
   // colors
   // other sprites: sleep, shower, feeding
   // sprites for age
   // points for living
   // minigames for different actions
}

int main()
{
   int id = -1;
   // instantiate pet health
   int hunger = 100;
   int happiness = 100;
   int cleanliness = 100;

   while (1)
   {
      
   }
   // while (1)
   // {
   //    test_start(&led);
   //    // bypass all cores
   //    frame.bypass(1);
   //    bar.bypass(1);
   //    gray.bypass(1);
   //    ghost.bypass(1);
   //    osd.bypass(1);
   //    mouse.bypass(1);
   //    sleep_ms(3000);

   //    // enable cores one by one
   //    frame_check(&frame);
   //    bar_check(&bar);
   //    gray_check(&gray);
   //    ghost_check(&ghost);
   //    osd_check(&osd);
   //    mouse_check(&mouse);
   //    while (sw.read(0))
   //    {
   //       // test composition with different overlays if sw(0) is 1
   //       mouse.bypass(sw.read(1));
   //       osd.bypass(sw.read(2));
   //       ghost.bypass(sw.read(3));
   //       gray.bypass(sw.read(6));
   //       bar.bypass(sw.read(7));
   //       frame.bypass(sw.read(8));
   //       // set osd background color to transparent
   //       osd.set_color(0x0f0, sw.read(9));
   //       // set color/animation of ghost sprite
   //       ghost.wr_ctrl(sw.read() >> 11);
   //    } // while
   // }    // while
} // main
