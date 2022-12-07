#include "gameplay.h"
#include "graphics.h"
#include <math.h>
#include <cstring>


void feed(health *bars, FrameCore *frame_p, OsdCore *osd_p)
{
   // draw dots for food
   change_scene(1, frame_p);
   bars->hunger = 10;
   for (int i = 0; i < 6; i++)
   {
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
   for (int i = 0; i < 6; i++)
   {
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
    for (int i = 0; i < 4; i++)
    {
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
   while (!reset)
   {
      osd_p->set_color(0xf00, 0x000); // red/black
      osd_p->wr_char(x - 1, y, 32);   // big exclamation
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

   // reset values and go to default scene
   change_scene(Default, frame_p);
   osd_p->set_color(0xfff, 0x000);

   bars->hunger = 10;
   bars->cleanliness = 10;
   bars->happiness = 10;

   timer_p->clear();
   timer_p->go();
}