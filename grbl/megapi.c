#include "grbl.h"

#ifdef CPU_MAP_2560_MEGAPI_BOARD

void megapi_driver_init()
{
  // ---------------------------------------------------------------------------
  // Laser hard off: Mega D10 = PB4 = OC2A
  // ---------------------------------------------------------------------------
  DDRB |= (1 << 4);
  PORTB &= ~(1 << 4);
  OCR2A = 0;
  TCCR2A &= ~(1 << COM2A1);

  // ---------------------------------------------------------------------------
  // X/Y enable pins
  // X_ENABLE = D35 = PC2
  // Y_ENABLE = D36 = PC1
  // Active LOW
  // ---------------------------------------------------------------------------
  DDRC |= (1 << 2) | (1 << 1);
  PORTC &= ~((1 << 2) | (1 << 1));

  // ---------------------------------------------------------------------------
  // X driver support pins
  // X_MS1   = D34 = PC3
  // X_MS2   = D33 = PC4
  // X_MS3   = D32 = PC5
  // X_RESET = D31 = PC6
  // X_SLEEP = D18 = PD3
  // ---------------------------------------------------------------------------
  DDRC |= (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
  DDRD |= (1 << 3);

  // ---------------------------------------------------------------------------
  // Y driver support pins
  // Y_MS1   = D37 = PC0
  // Y_MS2   = D40 = PG1
  // Y_MS3   = D41 = PG0
  // Y_RESET = D38 = PD7
  // Y_SLEEP = D19 = PD2
  // ---------------------------------------------------------------------------
  DDRC |= (1 << 0);
  DDRG |= (1 << 1) | (1 << 0);
  DDRD |= (1 << 7) | (1 << 2);

  // Microstepping HIGH.
  PORTC |= (1 << 3) | (1 << 4) | (1 << 5);
  PORTC |= (1 << 0);
  PORTG |= (1 << 1) | (1 << 0);

  // Wake drivers: RESET and SLEEP HIGH.
  PORTC |= (1 << 6);
  PORTD |= (1 << 3);
  PORTD |= (1 << 7) | (1 << 2);
}


void megapi_limits_init()
{
  // X = Mega D60 / A6 / PF6
  // Y = Mega D61 / A7 / PF7
  DDRF &= ~((1 << 6) | (1 << 7));
  PORTF |= ((1 << 6) | (1 << 7));
}


uint8_t megapi_limits_get_state()
{
  uint8_t limit_state = 0;
  uint8_t pin = PINF;

  if (pin & (1 << 6)) {
    limit_state |= (1 << X_AXIS);
  }

  if (pin & (1 << 7)) {
    limit_state |= (1 << Y_AXIS);
  }

  if (bit_istrue(settings.flags, BITFLAG_INVERT_LIMIT_PINS)) {
    limit_state ^= ((1 << X_AXIS) | (1 << Y_AXIS));
  }

  return limit_state;
}


void megapi_laser_stop()
{
  OCR2A = 0;
  TCCR2A &= ~(1 << COM2A1);

  DDRB |= (1 << 4);
  PORTB &= ~(1 << 4);
}


void megapi_laser_set_speed(uint16_t pwm_value)
{
  if (pwm_value == 0) {
    megapi_laser_stop();
    return;
  }

  if (pwm_value > 255) {
    pwm_value = 255;
  }

  DDRB |= (1 << 4);

  // Match Arduino analogWrite(10, value):
  // Timer2 Fast PWM, non-inverting OC2A, prescaler 64.
  TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2A1);
  TCCR2B = (1 << CS22);
  OCR2A = (uint8_t)pwm_value;
}


uint16_t megapi_compute_pwm_value(float rpm)
{
  if (rpm <= 0.0) {
    sys.spindle_speed = 0.0;
    return 0;
  }

  if (rpm > 255.0) {
    rpm = 255.0;
  }

  sys.spindle_speed = rpm;
  return (uint16_t)rpm;
}

#endif