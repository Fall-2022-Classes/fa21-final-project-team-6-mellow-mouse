/*****************************************************************/ /**
                                                                     * @file main_video_test.cpp
                                                                     *
                                                                     * @brief Basic test of 4 basic i/o cores
                                                                     *
                                                                     * @author p chu
                                                                     * @version v1.0: initial release
                                                                     *********************************************************************/

// #define _DEBUG
#include <math.h>
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include "spi_core.h"

/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p)
{
   int x, y;

   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x18); // animation; blue ghost
   x = 50;
   y = 100;
   ghost_p->move_xy(x, y);
   // for (int i = 0; i < 156; i++)
   // {
   //    ghost_p->move_xy(x, y);
   //    sleep_ms(100);
   //    x = x + 4;
   //    if (i == 80)
   //    {
   //       // change to red ghost half way
   //       ghost_p->wr_ctrl(0x00);
   //    }
   // }
   sleep_ms(3000);
}

// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));

// change the scene given a scene number
// let 0 be home/default
// let 1 be feed scene
// let 2 be shower scene
// let 3 be pet scene
enum Scene
{
   Default,
   Feed,
   Shower,
   Pet,
   GameOver
};

void change_scene(int scene)
{
   switch (scene)
   {
   case Default:
      break;

   case Feed:
      break;

   case Shower:
      break;

   case Pet:
      break;

   case GameOver:
      break;

   default:
      break;
   }
}

void feed(int *hunger)
{
   change_scene(1);

   change_scene(0);
}
void shower(int *cleanliness)
{
   change_scene(2);

   change_scene(0);
}
void pet(int *happiness)
{
   change_scene(3);

   change_scene(0); // may look terrible bc it would constantly be switching whenever we pet (unless background doesnt change and only sprite does)
}
void game_over()
{
   change_scene(GameOver);
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

template <typename T>
/**
 * @brief This is a struct to hold x, y, and z acceleration values and calculate
 * the normalized and magnitude of the values
 *
 */
struct acc_vals
{
   T x_acc, y_acc, z_acc;
   /**
    * @brief calculates the normalized acceleration reading
    *
    * @return acc_vals<float> Float version of the acc_vals struct; make sure
    * the type being assigned is also float
    * @note the struct can be improved with operators for comparisons and
    * assignment
    */
   acc_vals<float> calculate_normalized()
   {
      const float raw_max = 127.0 / 2.0; // 128 max 8-bit reading for +/-2g
      acc_vals<float> temp;
      temp.x_acc = static_cast<float>(x_acc) / raw_max;
      temp.y_acc = static_cast<float>(y_acc) / raw_max;
      temp.z_acc = static_cast<float>(z_acc) / raw_max;
      return temp;
   }
   /**
    * @brief calculates the magnitude of the three acceleration axis
    *
    * @return double magnitude of the accelerations
    */
   double abs_acc()
   {
      return sqrt(static_cast<int32_t>(x_acc) * x_acc +
                  static_cast<int32_t>(y_acc) * y_acc +
                  static_cast<int32_t>(z_acc) * z_acc);
   }
};

/**
 * @brief Get the difference of acceleration between 10ms
 * @note This might be too long to detect a tap. This also blocks until both
 * values are obtained and the difference is calculated
 *
 * @param spi_p spi core
 * @return acc_vals struct containing the difference in acceleration
 */
acc_vals<int8_t> get_difference(SpiCore *spi_p)
{
   const uint8_t RD_CMD = 0x0b;
   // const uint8_t PART_ID_REG = 0x02;
   const uint8_t DATA_REG = 0x08;
   acc_vals<int8_t> values;
   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);
   spi_p->assert_ss(0);       // activate
   spi_p->transfer(RD_CMD);   // for read operation
   spi_p->transfer(DATA_REG); //
   values.x_acc = spi_p->transfer(0x00);
   values.y_acc = spi_p->transfer(0x00);
   values.z_acc = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);
   // sleep for 100Hz sample frequency
   // The max might be 400Hz, but choose 100Hz for safety
   sleep_ms(10);
   spi_p->assert_ss(0);       // activate
   spi_p->transfer(RD_CMD);   // for read operation
   spi_p->transfer(DATA_REG); //
   values.x_acc -= spi_p->transfer(0x00);
   values.y_acc -= spi_p->transfer(0x00);
   values.z_acc -= spi_p->transfer(0x00);
   spi_p->deassert_ss(0);
   return values;
}

bool acc_check(SpiCore *spi_p)
{
   const int MAX = 100;
   if (get_difference(spi_p).abs_acc() > MAX)
   {
   }
   return 0;
}

struct health
{
   unsigned hunger = 100;
   unsigned happiness = 100;
   unsigned cleanliness = 100;
   int normalized_hunger() { return hunger / 10; }
   int normalized_happiness() { return happiness / 10; }
   int normalized_cleanliness() { return cleanliness / 10; }
};

// draw the bars (5 segments)
void draw_bars(OsdCore *osd_p)
{
   osd_p->bypass(0);
   osd_p->clr_screen();

   // bar parameters
   int num_tiles = 10;
   int space = 4;
   int bar2x = 35;
   int bar1x = bar2x - (num_tiles + space);
   int bar3x = bar2x + num_tiles + space;

   // brackets for bar limits
   osd_p->set_color(0xfff, 0x000); // white/black
   osd_p->wr_char(bar1x - 1, 21, '[', 0);
   osd_p->wr_char(bar1x + num_tiles, 21, ']', 0);
   osd_p->wr_char(bar2x - 1, 21, '[', 0);
   osd_p->wr_char(bar2x + num_tiles, 21, ']', 0);
   osd_p->wr_char(bar3x - 1, 21, '[', 0);
   osd_p->wr_char(bar3x + num_tiles, 21, ']', 0);

   // hunger bar
   osd_p->set_color(0xf00, 0x000); // red/black
   for (int i = bar1x; i < bar1x + num_tiles; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
   // cleanliness bar
   osd_p->set_color(0x0f0, 0x000); // green/black
   for (int i = bar2x; i < bar2x + num_tiles; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
   // happiness bar
   osd_p->set_color(0xff0, 0x000); // yellow/black
   for (int i = bar3x; i < bar3x + num_tiles; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
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
         game_over();
         // show restart button
      }
      // draw_bars();
      timer->clear();
   }
}

void pet_game(TimerCore *timer, Ps2Core *ps2_p, GpiCore *sw, SpiCore *spi_p, int *id, int *hunger, int *happiness, int *cleanliness)
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
      feed(hunger);
   }
   else if (keeb == 1)
   {
      shower(cleanliness);
   }
   else if (acc_check(spi_p))
   {
      pet(happiness);
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
   const int8_t MAX_TAP = 45;
   const acc_vals<int8_t> max_acceleration{MAX_TAP, 0, 0};

   int id = -1;
   // instantiate bars
   health bars;
   draw_bars(&osd);

   while (1)
   {
      ghost_check(&ghost);
      // ghost.wr_ctrl(sw.read() >> 11);
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

   //    } // while
   // }    // while
} // main
