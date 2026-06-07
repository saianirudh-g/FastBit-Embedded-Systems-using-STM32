/*
 * debug.h
 *
 *  Created on: 07-Jun-2026
 *      Author: saian
 */

#ifdef DEBUG

#include <stdint.h>

extern float d_roll_angle;
extern uint32_t d_roll_angle_filt;
extern float d_dt;

#ifdef DEBUG
#define DEBUG_ROLL_ANGLE(a)      (d_roll_angle = a)
#define DEBUG_ROLL_ANGLE_FILT(a) (d_roll_angle_filt = a)
#define DEBUG_DT(a)              (d_dt = a)
#else
#define DEBUG_ROLL_ANGLE(a)      (void)a
#define DEBUG_ROLL_ANGLE_FILT(a) (void)a
#define DEBUG_DT(a)              (void)a
#endif

#endif /* SRC_DEBUG_H_ */
