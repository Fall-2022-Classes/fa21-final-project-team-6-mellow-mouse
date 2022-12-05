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
void draw_bars(OsdCore *osd_p, health *bars);

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

/* 
   smiley = 0, 1
   big exlamation 32
*/
void feed(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   // draw dots for food
   change_scene(1, frame_p);
   bars->hunger = 10;
   // for (int i = 0; i < 6; i++)
   // {
   //    osd_p->wr_char(32 + i, 14, 7); // dots = 6,7,8,9
   //    sleep_ms(500);
   //    osd_p->clr_screen();
   //    draw_bars(osd_p, bars);
   // }
   sleep_ms(3000);
   change_scene(0, frame_p);
}
void shower(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   // draw water drops and maybe music note?
   change_scene(2, frame_p);
   bars->cleanliness = 10;
   for (int i = 0; i < 6; i++)
   {
      // music notes
      osd_p->wr_char(20 + i, 15, 14); // music notes = 13, 14
      // water
      osd_p->wr_char(34, 2 + i, 6);
      osd_p->wr_char(30, 0 + i, 6);
      osd_p->wr_char(38, 4 + i, 6);
      osd_p->wr_char(42, 6 + i, 6);
      sleep_ms(500);
      osd_p->clr_screen();
      draw_bars(osd_p, bars);
   }
   change_scene(0, frame_p);
}
void pet(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   change_scene(3, frame_p);
   bars->happiness = 10;
   for (int i = 0; i < 4; i++)
   {
      osd_p->wr_char(38 + i, 12, 2); // heart = 2
      sleep_ms(1000);
   }
   
   change_scene(0, frame_p); // may look terrible bc it would constantly be switching whenever we pet (unless background doesnt change and only sprite does)
}
void game_over(FrameCore *frame_p, OsdCore *osd_p, Ps2Core *ps2_p, health *bars, TimerCore *timer_p)
{
   bool reset = false;
   change_scene(GameOver, frame_p);
   timer_p->pause();
   int x = 20;
   int y = 15;
   char gameover[] = "GAME OVER";
   while (!reset)
   {
      osd_p->set_color(0xf00, 0x000); // red/black
      osd_p->wr_char(x - 1, y, 32); // big exclamation
      for (size_t i = 0; i < strlen(gameover); i++)
      {
         osd_p->wr_char(x + i, y, gameover[i]);
      }
      // draw exclamation marks
      char ch = 0;
      int keeb = ps2_p->get_kb_ch(&ch);
      if (keeb == 1)
      {
         if (ch == ' ')
         {
            reset = true;
         }
      }
   }
   // delete game over text
   // for (size_t i = 0; i < strlen(gameover); i++)
   // {
   //    osd_p->wr_char(x + i, y, '\0');
   // }
   // reset values and go to default scene
   change_scene(Default, frame_p);
   osd_p->set_color(0xfff, 0x000);

   bars->hunger = 10;
   bars->cleanliness = 10;
   bars->happiness = 10;

   timer_p->clear();
   timer_p->go();
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
         else if (ch == 'p')
         {
            return 2;
         }
         else if (ch == 'r')
         {
            return 3;
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
   const int MAX = 10;
   // uart.disp(get_difference(spi_p).abs_acc());
   if (get_difference(spi_p).abs_acc() > MAX)
   {
      // uart.disp("got a difference greater than max \r\n");
      return 1;
   }
   else
   {
      // uart.disp("didn't get a difference greater than max \r\n");
      return 0;
   }
}

// draw the bars (5 segments)
void draw_bars(OsdCore *osd_p, health *bars)
{
   osd_p->clr_screen();
   // bar parameters
   unsigned num_tiles = 10;         // length of each bar
   unsigned space = 6;              // space between bars
   unsigned cleanliness_bar_x = 34; // cleanliness bar x origin
   unsigned hunger_bar_x = cleanliness_bar_x - (num_tiles + space);
   unsigned happiness_bar_x = cleanliness_bar_x + num_tiles + space;

   // brackets for bar limits
   osd_p->set_color(0xfff, 0x000);
   osd_p->wr_char(hunger_bar_x - 1, 21, '[', 0);
   osd_p->wr_char(hunger_bar_x + num_tiles, 21, ']', 0);
   osd_p->wr_char(cleanliness_bar_x - 1, 21, '[', 0);
   osd_p->wr_char(cleanliness_bar_x + num_tiles, 21, ']', 0);
   osd_p->wr_char(happiness_bar_x - 1, 21, '[', 0);
   osd_p->wr_char(happiness_bar_x + num_tiles, 21, ']', 0);

   // hunger bar
   for (unsigned i = hunger_bar_x; i < hunger_bar_x + bars->hunger; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
   }
   // cleanliness bar
   for (unsigned i = cleanliness_bar_x; i < cleanliness_bar_x + bars->cleanliness; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
   }
   // happiness bar
   for (unsigned i = happiness_bar_x; i < happiness_bar_x + bars->happiness; i++)
   {
      osd_p->wr_char(i, 21, ' ', 1);
   }

   // text over bars (have to redraw bc osd gets cleared when bars refresh)
   char hunger[] = "HUNGER";
   char happiness[] = "HAPPINESS";
   char cleanliness[] = "CLEANLINESS";

   for (size_t i = 0; i < strlen(hunger); i++)
   {
      osd_p->wr_char(hunger_bar_x + i, 20, hunger[i]);
   }
   for (size_t i = 0; i < strlen(cleanliness); i++)
   {
      osd_p->wr_char(cleanliness_bar_x + i, 20, cleanliness[i]);
   }
   for (size_t i = 0; i < strlen(happiness); i++)
   {
      osd_p->wr_char(happiness_bar_x + i, 20, happiness[i]);
   }
}

char get_digit(uint64_t number, int index)
{
   int dividend = pow(10, index);
   int digit = (number / dividend) % 10;
   return digit + '0';
}
void reset_health(health *bars)
{
   bars->happiness = 10;
   bars->cleanliness = 10;
   bars->hunger = 10;
}
void draw_timer(TimerCore *timer, GpiCore *sw, OsdCore *osd_p)
{
   // display timer and max_time
   uint64_t time = timer->read_time();
   uint64_t max_time = sw->read();
   if (max_time < 5000000)
   { // 1s threshold for max time
      max_time = 5000000;
   }
   for (int i = 0; i < 8; i++)
   {
      osd_p->wr_char(13 - i, 2, get_digit(time, i), 0);
      osd_p->wr_char(13 - i, 3, get_digit(max_time, i), 0);
   }
}

void check_pet_health(TimerCore *timer_p, GpiCore *sw_p, health *bars, OsdCore *osd_p, FrameCore *frame_p, Ps2Core *ps2_p)
{
   uint64_t time = timer_p->read_time();
   uint64_t max_time = sw_p->read();

   if (max_time < 5000000)
   { // 5s threshold for max time
      max_time = 5000000;
   }
   draw_timer(timer_p, sw_p, osd_p);
   if (time >= max_time)
   {
      bars->hunger--;
      bars->happiness--;
      bars->cleanliness--;

      draw_bars(osd_p, bars);
      if (bars->hunger == 0 || bars->happiness == 0 || bars->cleanliness == 0)
      {
         // display game over screen, end game
         game_over(frame_p, osd_p, ps2_p, bars, timer_p);
      }
      timer_p->clear();
      draw_bars(osd_p, bars);
   }
}

void draw_sprite(SpriteCore *hampster_p)
{
   hampster_p->wr_ctrl(0x10);
   hampster_p->move_xy(640 / 2 - 32, 480 / 2 - 32);
}

// main game cycle
void game_cycle(TimerCore *timer_p, Ps2Core *ps2_p, GpiCore *sw_p, SpiCore *spi_p, SpriteCore *ghost_p, int *id, health *bars, OsdCore *osd_p, FrameCore *frame_p)
{

   // decrement bars
   check_pet_health(timer_p, sw_p, bars, osd_p, frame_p, ps2_p);

   // check for user input
   int keeb = keyboard_check(ps2_p, id);
   bool greater_than_acc = acc_check(spi_p);
   if (keeb == -1)
   {
      // invalid
   }
   else if (keeb == 0)
   {
      feed(bars, frame_p, osd_p);
   }
   else if (keeb == 1)
   {
      shower(bars, frame_p, osd_p);
   }
   else if (keeb == 2)
   {
      pet(bars, frame_p, osd_p);
   }

   if(greater_than_acc == 1){
      pet(bars, frame_p, osd_p);
   }

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
   osd.bypass(0);
   osd.clr_screen();
   timer.clear();
   timer.go();
   
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
