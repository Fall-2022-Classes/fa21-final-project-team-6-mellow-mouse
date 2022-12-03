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
#include <cstring>
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include "spi_core.h"
void ps2_check(Ps2Core *ps2_p)
{
   int id;
   int lbtn, rbtn, xmov, ymov;
   char ch;
   unsigned long last;

   uart.disp("\n\rPS2 device (1-keyboard / 2-mouse): ");
   id = ps2_p->init();
   uart.disp(id);
   uart.disp("\n\r");
   last = now_ms();
   do
   {
      if (id == 2)
      { // mouse
         if (ps2_p->get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov))
         {
            uart.disp("[");
            uart.disp(lbtn);
            uart.disp(", ");
            uart.disp(rbtn);
            uart.disp(", ");
            uart.disp(xmov);
            uart.disp(", ");
            uart.disp(ymov);
            uart.disp("] \r\n");
            last = now_ms();

         } // end get_mouse_activitiy()
      }
      else
      {
         if (ps2_p->get_kb_ch(&ch))
         {
            uart.disp(ch);
            uart.disp(" ");
            last = now_ms();
         } // end get_kb_ch()
      }    // end id==2
   } while (now_ms() - last < 5000);
   uart.disp("\n\rExit PS2 test \n\r");
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

/**********************************************************************
 * enums and structs
 **********************************************************************/
enum Scene
{
   Default,
   Feed,
   Shower,
   Pet,
   GameOver
};

struct health
{
   unsigned hunger = 10;
   unsigned happiness = 10;
   unsigned cleanliness = 10;
};

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

/**********************************************************************
 * Functions
 **********************************************************************/

void change_scene(int scene, FrameCore *frame_p)
{
   switch (scene)
   {
   case Default:
      frame_p->clr_screen(0x444);
      break;

   case Feed:
      frame_p->clr_screen(0x630);
      break;

   case Shower:
      frame_p->clr_screen(0x0FF);
      break;

   case Pet:
      frame_p->clr_screen(0xF0F);
      break;

   case GameOver:
      frame_p->clr_screen(0x000);
      break;

   default:
      break;
   }
}

void feed(health *bars, FrameCore *frame_p)
{
   change_scene(1, frame_p);
   (*bars).hunger = 10;
   sleep_ms(3000);
   change_scene(0, frame_p);
}
void shower(health *bars, FrameCore *frame_p)
{
   change_scene(2, frame_p);
   (*bars).cleanliness = 10;
   sleep_ms(3000);
   change_scene(0, frame_p);
}
void pet(health *bars, FrameCore *frame_p)
{
   change_scene(3, frame_p);
   (*bars).happiness = 10;
   change_scene(0, frame_p); // may look terrible bc it would constantly be switching whenever we pet (unless background doesnt change and only sprite does)
}
void game_over(FrameCore *frame_p, OsdCore *osd_p, Ps2Core *ps2_p)
{
   bool reset = false;
   while(!reset){
      int x = 7;
      int y = 30;
      char gameover[] = "GAME OVER";
      for (size_t i = 0; i < strlen(gameover); i++)
      {
         osd_p->wr_char(x+i, y, gameover[i]);
      }

      char ch = 0;
      int keeb = ps2_p->get_kb_ch(&ch);
      if(keeb == ' '){
         reset = true;
      }
   }
   change_scene(GameOver, frame_p);
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

// checks for an acceleration value that's greater than a specified value
bool acc_check(SpiCore *spi_p)
{
   const int MAX = 50;
   if (get_difference(spi_p).abs_acc() > MAX)
   {
      return 1;
   }
   return 0;
}

// draw the bars (5 segments)
void draw_bars(OsdCore *osd_p, health *bars)
{
   osd_p->bypass(0);
   osd_p->clr_screen();

   // bar parameters
   unsigned num_tiles = 10;
   unsigned space = 4;  // space between bars
   unsigned bar2x = 37; // bar 2 x origin
   unsigned bar1x = bar2x - (num_tiles + space);
   unsigned bar3x = bar2x + num_tiles + space;

   // brackets for bar limits
   osd_p->set_color(0xf00, 0x000); // red/black
   osd_p->wr_char(bar1x - 1, 21, '[', 0);
   osd_p->wr_char(bar1x + num_tiles, 21, ']', 0);
   osd_p->wr_char(bar2x - 1, 21, '[', 0);
   osd_p->wr_char(bar2x + num_tiles, 21, ']', 0);
   osd_p->wr_char(bar3x - 1, 21, '[', 0);
   osd_p->wr_char(bar3x + num_tiles, 21, ']', 0);

   // hunger bar
   for (unsigned i = bar1x; i < bar1x + (*bars).hunger; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
   // cleanliness bar
   for (unsigned i = bar2x; i < bar2x + (*bars).cleanliness; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
   // happiness bar
   for (unsigned i = bar3x; i < bar3x + (*bars).happiness; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
      sleep_ms(100);
   }
}

char get_digit(uint64_t number, int index)
{
   int dividend = pow(10, index);
   int digit = (number / dividend) % 10;
   return digit + '0';
}

void check_pet_health(TimerCore *timer, GpiCore *sw, health *bars, OsdCore *osd_p, FrameCore *frame_p, Ps2Core *ps2_p)
{
   uint64_t time = timer->read_time();
   uint64_t max_time = sw->read();

   if (max_time < 1000000) { // 1s threshold for max time
      max_time = 1000000;
   }

   // display timer and max_time
   osd_p->set_color(0xfff, 0x000);
   osd_p->bypass(0);

   for (int i = 0; i < 5; i++)
   {
      osd_p->wr_char(13 - i, 2, get_digit(time, i), 0);
      osd_p->wr_char(13 - i, 3, get_digit(max_time, i), 0);
      sleep_ms(100);
   }

   if (time >= max_time)
   {
      (*bars).hunger--;
      (*bars).happiness--;
      (*bars).cleanliness--;

      if ((*bars).hunger == 0 || (*bars).happiness == 0 || (*bars).cleanliness == 0)
      {
         // display game over screen, end game
         game_over(frame_p, osd_p, ps2_p);
         timer->pause();
         // show restart button
      }

      draw_bars(osd_p, bars);
      timer->clear();
   }
}

void draw_sprite(SpriteCore *hampster_p)
{
   hampster_p->wr_ctrl(0x10);
   hampster_p->move_xy(640/2, 480/2);
}

// main game cycle
void game_cycle(TimerCore *timer, Ps2Core *ps2_p, GpiCore *sw, SpiCore *spi_p, SpriteCore *ghost_p, int *id, health *bars, OsdCore *osd_p, FrameCore *frame_p)
{

   // decrement bars
   check_pet_health(timer, sw, bars, osd_p, frame_p, ps2_p);

   // check for user input
   int keeb = keyboard_check(ps2_p, id);
   if (keeb == -1)
   {
      // invalid
   }
   else if (keeb == 0)
   {
      feed(bars, frame_p);
   }
   else if (keeb == 1)
   {
      shower(bars, frame_p);
   }
   else if (acc_check(spi_p))
   {
      pet(bars, frame_p);
   }

   // accelerometer check
   // if past some threshold, pet screen

   // extra:
   // achievements?
   // colors
   // other sprites: sleep, shower, feeding
   // sprites for age
   // points for living
   // minigames for different actions
}

// external core instantiation
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore hampster(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
TimerCore timer(get_slot_addr(BRIDGE_BASE, S0_SYS_TIMER));

int main()
{
   mouse.bypass(1);
   timer.go();
   const int8_t MAX_TAP = 45;
   const acc_vals<int8_t> max_acceleration{MAX_TAP, 0, 0};

   int id = -1;
   // instantiate bars
   health bars;
   draw_sprite(&hampster);
   draw_bars(&osd, &bars);
   change_scene(Default, &frame);

   while (1)
   {
      // ghost_check(&ghost);
      game_cycle(&timer, &ps2, &sw, &spi, &hampster, &id, &bars, &osd, &frame);
      // ghost.wr_ctrl(sw.read() >> 11);
   }

} // main
