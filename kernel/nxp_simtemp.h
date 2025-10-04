#ifndef _NXP_SIMTMEP_
#define _NXP_SIMTMEP_

#include <linux/types.h>

typedef enum {
    NORMAL,
    NOISE,
    RAMP,
    SYS_MAX_MODE
}sys_mode_t;

struct simtemp_cfg {
  sys_mode_t  sys_mode;
  u16 sampling_rate_ms;
  s32 temp_threshold;
  u32 stats
};

#endif