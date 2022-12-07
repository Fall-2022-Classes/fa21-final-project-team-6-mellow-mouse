#ifndef SRC_GAMEPLAY_H_
#define SRC_GAMEPLAY_H_

#include "vga_core.h"
#include "types.h"
#include "gpio_cores.h"
#include "ps2_core.h"
#include "spi_core.h"

void feed(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void shower(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void pet(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void game_over(FrameCore *frame_p, OsdCore *osd_p, Ps2Core *ps2_p, health *bars, TimerCore *timer_p);
double map(double x, double in_min, double in_max, double out_min, double out_max);
// return 0 if 'f', return 1 if 's', return -1 if none
int keyboard_check(Ps2Core *ps2_p, int *id);
/**
 * @brief Get the difference of acceleration between 10ms
 * @note This might be too long to detect a tap. This also blocks until both
 * values are obtained and the difference is calculated
 *
 * @param spi_p spi core
 * @return acc_vals struct containing the difference in acceleration
 */
bool acc_check(SpiCore *spi_p);

#endif  // SRC_GAMEPLAY_H_