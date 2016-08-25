/*
 * serv.c
 *
 *  Created on: 2016年7月27日
 *      Author: 李文胜
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <mraa.h>
#include "servo.h"
mraa_result_t haltPwm() {
	return mraa_pwm_enable(m_pwmServoContext, 0);
}
int calcPulseTraveling(int value) {
	// if bigger than the boundaries
	if (value > m_maxAngle) {
		return m_maxPulseWidth;
	}

	// if less than the boundaries
	if (value < 0) {
		return m_minPulseWidth;
	}

	// the conversion
	return (int) ((float) m_minPulseWidth
			+ ((float) value / m_maxAngle)
					* ((float) m_maxPulseWidth - (float) m_minPulseWidth));
}
int setAngle(int angle, int pin) {
	if (angle > m_maxAngle || angle < 0) {
		return MRAA_ERROR_UNSPECIFIED;
	}

	if (pin == pin3) {
		mraa_pwm_enable(m_pwmServoContext, 1);
		mraa_pwm_period_us(m_pwmServoContext, m_period);
		mraa_pwm_pulsewidth_us(m_pwmServoContext, calcPulseTraveling(angle));
	}
	if (pin == pin9) {
		mraa_pwm_enable(m_pwmServoContext1, 1);
		mraa_pwm_period_us(m_pwmServoContext1, m_period);
		mraa_pwm_pulsewidth_us(m_pwmServoContext1, calcPulseTraveling(angle));
	}

	if (m_waitAndDisablePwm) {
		sleep(1); // we must make sure that we don't turn off PWM before the servo is done moving.
		haltPwm();
	}

	m_currAngle = angle;
	return MRAA_SUCCESS;
}
void init(int pin1, int pin2, int minPulseWidth, int maxPulseWidth,
		int waitAndDisablePwm) {
	m_minPulseWidth = minPulseWidth;
	m_maxPulseWidth = maxPulseWidth;
	m_period = PERIOD;

	m_waitAndDisablePwm = waitAndDisablePwm;

	m_maxAngle = 180.0;

	m_pwmServoContext = mraa_pwm_init(pin1);
	m_pwmServoContext1 = mraa_pwm_init(pin2);

	m_currAngle = 180;

//	setAngle(0, pin3);
//	setAngle(0, pin9);
}
void servo(int angle,int pin){
	pin3 = 3;
	pin9 = 9;
	init(pin3, pin9, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH,
			DEFAULT_WAIT_DISABLE_PWM);
			setAngle(angle,pin);
}

