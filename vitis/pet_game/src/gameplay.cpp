#include "gameplay.h"
#include "graphics.h"
#include <math.h>
#include <cstring>

void feed(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   // draw dots for food
   change_scene(1, frame_p);
   bars->hunger = 10;
   char feeding[] = "Feeding...";
   for (int i = 0; i < 6; i++)
   {
      for (size_t i = 0; i < strlen(feeding); i++)
      {
         osd_p->wr_char(35 + i, 5, feeding[i]);
      }
      osd_p->wr_char(33 + i, 15, 7); // dots = 6,7,8,9
      sleep_ms(500);
      osd_p->clr_screen();
      draw_bars(osd_p, bars);
   }
   change_scene(0, frame_p);
}

void shower(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   // draw water drops and maybe music note?
   change_scene(2, frame_p);
   bars->cleanliness = 10;
   char showering[] = "Showering...";
   for (int i = 0; i < 6; i++)
   {
      for (size_t i = 0; i < strlen(showering); i++)
      {
         osd_p->wr_char(35 + i, 5, showering[i]);
      }
      // music notes
      osd_p->wr_char(38 - (i % 3), 13 - (i % 3), 14); // music notes = 13, 14
      // water
      osd_p->wr_char(36, 10 + i, 9);
      osd_p->wr_char(38, 8 + i, 9);
      osd_p->wr_char(40, 9 + i, 9);
      osd_p->wr_char(42, 8 + i, 9);
      osd_p->wr_char(44, 10 + i, 9);
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
   char petting[] = "Petting...";
   for (int i = 0; i < 4; i++)
   {
      for (size_t i = 0; i < strlen(petting); i++)
      {
         osd_p->wr_char(35 + i, 5, petting[i]);
      }
      osd_p->wr_char(38 + i, 12, 2); // heart = 2
      sleep_ms(1000);
   }
   change_scene(0, frame_p);
   osd_p->clr_screen();
   draw_bars(osd_p, bars);
}

void game_over(FrameCore *frame_p, OsdCore *osd_p, Ps2Core *ps2_p, health *bars, TimerCore *timer_p)
{
   bool reset = false;
   change_scene(GameOver, frame_p);
   timer_p->pause();
   int x = 20;
   int y = 15;
   char gameover[] = "GAME OVER";
   char space[] = "Press Space Bar to reset...";
   while (!reset)
   {
      osd_p->set_color(0xf00, 0x000); // red/black
      osd_p->wr_char(x - 1, y, 32);   // big exclamation
      for (size_t i = 0; i < strlen(gameover); i++)
      {
         osd_p->wr_char(x + i, y, gameover[i]);
      }
      for (size_t i = 0; i < strlen(space); i++)
      {
         osd_p->wr_char(x + i, 6, space[i]);
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

   // reset values and go to default scene
   change_scene(Default, frame_p);
   osd_p->set_color(0xfff, 0x000);

   bars->hunger = 10;
   bars->cleanliness = 10;
   bars->happiness = 10;

   timer_p->clear();
   timer_p->go();
}


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
   const int MAX = 20;
   if (get_difference(spi_p).abs_acc() > MAX)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

double map(double x, double in_min, double in_max, double out_min,
           double out_max)
{
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}