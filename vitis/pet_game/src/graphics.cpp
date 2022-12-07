#include "graphics.h"
#include "types.h"
#include <math.h>
#include <cstring>


// helpers
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

// main functions
void draw_sprite(SpriteCore *hampster_p)
{
   hampster_p->wr_ctrl(0x10);
   hampster_p->move_xy(640 / 2 - 32, 480 / 2 - 32);
}

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

   // controls
   char feed[] = "press 'f' to feed";
   char shower[] = "press 's' to shower";
   char pet[] = "tap, shake, or press 'p' to pet";
   for (size_t i = 0; i < strlen(feed); i++)
   {
      osd_p->wr_char(hunger_bar_x + i, 24, feed[i]);
   }
   for (size_t i = 0; i < strlen(shower); i++)
   {
      osd_p->wr_char(hunger_bar_x + i, 25, shower[i]);
   }
   for (size_t i = 0; i < strlen(pet); i++)
   {
      osd_p->wr_char(hunger_bar_x + i, 26, pet[i]);
   }
}

void draw_timer(uint64_t max_time, uint64_t time, GpiCore *sw, OsdCore *osd_p)
{
   // display timer and max_time
   // uint64_t time = timer->read_time();
   // uint64_t max_time = sw->read();
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