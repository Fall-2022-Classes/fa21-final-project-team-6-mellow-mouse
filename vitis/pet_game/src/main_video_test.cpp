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

#include "types.h"
#include "graphics.h"
#include "gameplay.h"

void draw_achievement(const unsigned *total_cycles, OsdCore *osd_p)
{
   if (*total_cycles > 9)
   {
      char text[] = "Achievement: Live for 10 cycles";
      for (int i = 0; i < strlen(text); i++)
      {
         osd_p->wr_char(2, 2, text[i]);
      }
   }
}

void check_pet_health(TimerCore *timer_p, GpiCore *sw_p, health *bars, OsdCore *osd_p, FrameCore *frame_p, Ps2Core *ps2_p, unsigned *total_cycles)
{
   uint64_t time = timer_p->read_time();
   uint64_t max_time = map(sw_p->read(), 0, 65536 - 1, 5000000, 20000000);

   if (time >= max_time)
   {
      bars->hunger--;
      bars->happiness--;
      bars->cleanliness--;
      (*total_cycles)++;

      draw_bars(osd_p, bars);
      // draw_achievement(total_cycles, osd_p);
      if (bars->hunger == 0 || bars->happiness == 0 || bars->cleanliness == 0)
      {
         // display game over screen, end game
         game_over(frame_p, osd_p, ps2_p, bars, timer_p);
         *total_cycles = 0;
      }
      timer_p->clear();
      draw_bars(osd_p, bars);
   }
}

// main game cycle
void game_cycle(TimerCore *timer_p, Ps2Core *ps2_p, GpiCore *sw_p, SpiCore *spi_p, SpriteCore *ghost_p, int *id, health *bars, OsdCore *osd_p, FrameCore *frame_p, unsigned *total_cycles)
{

   // decrement bars
   check_pet_health(timer_p, sw_p, bars, osd_p, frame_p, ps2_p, total_cycles);

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

   if (greater_than_acc == 1)
   {
      pet(bars, frame_p, osd_p);
   }
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
   // init values and cores
   unsigned total_cycles = 0;
   int id = -1;
   mouse.bypass(1);
   osd.bypass(0);
   osd.clr_screen();
   timer.clear();
   timer.go();

   // instantiate bars and draw graphics
   health bars;
   draw_sprite(&hampster);
   draw_bars(&osd, &bars);
   change_scene(Default, &frame);

   while (1)
   {
      game_cycle(&timer, &ps2, &sw, &spi, &hampster, &id, &bars, &osd, &frame, &total_cycles);
   }

} // main
