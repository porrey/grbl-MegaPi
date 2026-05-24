/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2012-2017 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"


static float pwm_gradient; // Precalulated value to speed up rpm to PWM conversions.


void spindle_init()
{
  // Makeblock LaserBot safety: force Arduino Mega D10 / PB4 low at boot.
  DDRB |= (1 << 4);
  PORTB &= ~(1 << 4);

  // Configure variable spindle PWM and enable pin, if required.
  SPINDLE_PWM_DDR |= (1<<SPINDLE_PWM_BIT); // Configure as PWM output pin.
  SPINDLE_TCCRA_REGISTER = SPINDLE_TCCRA_INIT_MASK; // Configure PWM output compare timer
  SPINDLE_TCCRB_REGISTER = SPINDLE_TCCRB_INIT_MASK;
  SPINDLE_OCRA_REGISTER = SPINDLE_OCRA_TOP_VALUE; // Set the top value for 16-bit fast PWM mode
  SPINDLE_ENABLE_DDR |= (1<<SPINDLE_ENABLE_BIT); // Configure as output pin.
  SPINDLE_DIRECTION_DDR |= (1<<SPINDLE_DIRECTION_BIT); // Configure as output pin.

  pwm_gradient = SPINDLE_PWM_RANGE/(settings.rpm_max-settings.rpm_min);
  spindle_stop();

  // Makeblock LaserBot safety: force D10 off after GRBL timer setup.
  OCR2A = 0;
  TCCR2A &= ~(1 << COM2A1);
  DDRB |= (1 << 4);
  PORTB &= ~(1 << 4);
}


uint8_t spindle_get_state()
{
  #ifdef INVERT_SPINDLE_ENABLE_PIN
    if (bit_isfalse(SPINDLE_ENABLE_PORT,(1<<SPINDLE_ENABLE_BIT)) && (SPINDLE_TCCRA_REGISTER & (1<<SPINDLE_COMB_BIT))) {
  #else
    if (bit_istrue(SPINDLE_ENABLE_PORT,(1<<SPINDLE_ENABLE_BIT)) && (SPINDLE_TCCRA_REGISTER & (1<<SPINDLE_COMB_BIT))) {
  #endif
    if (SPINDLE_DIRECTION_PORT & (1<<SPINDLE_DIRECTION_BIT)) { return(SPINDLE_STATE_CCW); }
    else { return(SPINDLE_STATE_CW); }
  }
	return(SPINDLE_STATE_DISABLE);
}

void spindle_stop()
{
  OCR2A = 0;
  TCCR2A &= ~(1 << COM2A1);

  DDRB |= (1 << 4);
  PORTB &= ~(1 << 4);

  #ifdef INVERT_SPINDLE_ENABLE_PIN
    SPINDLE_ENABLE_PORT |= (1 << SPINDLE_ENABLE_BIT);
  #else
    SPINDLE_ENABLE_PORT &= ~(1 << SPINDLE_ENABLE_BIT);
  #endif
}


// Disables the spindle and sets PWM output to zero when PWM variable spindle speed is enabled.
// Called by various main program and ISR routines. Keep routine small, fast, and efficient.
// Called by spindle_init(), spindle_set_speed(), spindle_set_state(), and mc_reset().
void spindle_set_speed(uint16_t pwm_value)
{
  if (pwm_value == 0) {
    spindle_stop();
    return;
  }

  if (pwm_value > 255) {
    pwm_value = 255;
  }

  // Arduino Mega D10 = PB4 = OC2A.
  DDRB |= (1 << 4);

  // Match Arduino analogWrite(10, value) behavior:
  // Timer2 Fast PWM, non-inverting OC2A, prescaler 64.
  TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2A1);
  TCCR2B = (1 << CS22);
  OCR2A = (uint8_t)pwm_value;

  #ifdef INVERT_SPINDLE_ENABLE_PIN
    SPINDLE_ENABLE_PORT &= ~(1 << SPINDLE_ENABLE_BIT);
  #else
    SPINDLE_ENABLE_PORT |= (1 << SPINDLE_ENABLE_BIT);
  #endif
}

uint16_t spindle_compute_pwm_value(float rpm)
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

// Immediately sets spindle running state with direction and spindle rpm via PWM, if enabled.
// Called by g-code parser spindle_sync(), parking retract and restore, g-code program end,
// sleep, and spindle stop override.
void spindle_set_state(uint8_t state, float rpm)
{
  if (sys.abort) { return; } // Block during abort.

  if (state == SPINDLE_DISABLE) {
    sys.spindle_speed = 0.0;
    spindle_stop();

  } else {

    if (state == SPINDLE_ENABLE_CW) {
      SPINDLE_DIRECTION_PORT &= ~(1 << SPINDLE_DIRECTION_BIT);
    } else {
      SPINDLE_DIRECTION_PORT |= (1 << SPINDLE_DIRECTION_BIT);
    }

    // In laser mode, do not allow M4/CCW to turn the laser on.
    // M3/CW uses the requested S/rpm value.
    if (settings.flags & BITFLAG_LASER_MODE) {
      if (state == SPINDLE_ENABLE_CCW) {
        rpm = 0.0;
      }
    }

    // Makeblock LaserBot / MLaser:
    // Treat the incoming GRBL S value directly as 0-255 PWM power.
    //
    // Required GRBL settings:
    //   $30=255
    //   $31=0
    //   $32=1
    if (rpm <= 0.0) {
      sys.spindle_speed = 0.0;
      spindle_set_speed(0);
    } else {
      if (rpm > 255.0) {
        rpm = 255.0;
      }

      sys.spindle_speed = rpm;
      spindle_set_speed((uint16_t)rpm);
    }

    #ifndef SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED
      #ifdef INVERT_SPINDLE_ENABLE_PIN
        SPINDLE_ENABLE_PORT &= ~(1 << SPINDLE_ENABLE_BIT);
      #else
        SPINDLE_ENABLE_PORT |= (1 << SPINDLE_ENABLE_BIT);
      #endif
    #endif
  }

  sys.report_ovr_counter = 0; // Set to report change immediately.
}


// G-code parser entry-point for setting spindle state. Forces a planner buffer sync and bails 
// if an abort or check-mode is active.
void spindle_sync(uint8_t state, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
  spindle_set_state(state,rpm);
}
