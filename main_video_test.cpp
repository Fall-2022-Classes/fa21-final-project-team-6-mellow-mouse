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
#include "types.h"

#include "graphics.h"
#include "gameplay.h"


// check the keyboard input
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
   const int MAX = 20;
   // uart.disp(get_difference(spi_p).abs_acc());
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

void check_pet_health(TimerCore *timer_p, GpiCore *sw_p, health *bars, OsdCore *osd_p, FrameCore *frame_p, Ps2Core *ps2_p)
{
   uint64_t time = timer_p->read_time();
   uint64_t max_time = map(sw_p->read(), 0, 65536 - 1, 5000000, 20000000);

   draw_timer(max_time, time, sw_p, osd_p);
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
   // initialize values
   mouse.bypass(1);
   osd.bypass(0);
   osd.clr_screen();
   timer.clear();
   timer.go();

   int id = -1;
   // instantiate bars and graphics
   health bars;
   draw_sprite(&hampster);
   draw_bars(&osd, &bars);
   change_scene(Default, &frame);

   while (1)
   {
      game_cycle(&timer, &ps2, &sw, &spi, &hampster, &id, &bars, &osd, &frame);
   }

} // main
