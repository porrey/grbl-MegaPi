#ifndef makeblock_megapi_h
#define makeblock_megapi_h

#ifdef CPU_MAP_2560_MEGAPI_BOARD

void megapi_driver_init(void);

void megapi_limits_init(void);
uint8_t megapi_limits_get_state(void);

void megapi_laser_stop(void);
void megapi_laser_set_speed(uint16_t pwm_value);
uint16_t megapi_compute_pwm_value(float rpm);

#endif

#endif