/**********************************************************************
 * enums and structs
 **********************************************************************/
#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include "chu_init.h"

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

#endif  // SRC_TYPES_H_