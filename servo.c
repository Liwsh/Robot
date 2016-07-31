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
#include <servo.h>
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
int setAngle(int angle) {
	if (angle > m_maxAngle || angle < 0) {
		return MRAA_ERROR_UNSPECIFIED;
	}

	mraa_pwm_enable(m_pwmServoContext, 1);
	mraa_pwm_period_us(m_pwmServoContext, m_period);
	mraa_pwm_pulsewidth_us(m_pwmServoContext, calcPulseTraveling(angle));

	if (m_waitAndDisablePwm) {
		sleep(1); // we must make sure that we don't turn off PWM before the servo is done moving.
		haltPwm();
	}

	m_currAngle = angle;
	return MRAA_SUCCESS;
}
void init(int pin, int minPulseWidth, int maxPulseWidth, int waitAndDisablePwm) {
	m_minPulseWidth = minPulseWidth;
	m_maxPulseWidth = maxPulseWidth;
	m_period = PERIOD;

	m_waitAndDisablePwm = waitAndDisablePwm;

	m_maxAngle = 180.0;
	m_servoPin = pin;

	if (!(m_pwmServoContext = mraa_pwm_init(m_servoPin))) {
		printf("mraa_pwm_init() failed, invalid pin?");
		return;
	}

	m_currAngle = 180;

	setAngle(0);
}
int main() {
	init(9, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, DEFAULT_WAIT_DISABLE_PWM);
	while (1) {
		while (1) {
			setAngle(180);
			printf("Set angle to 180");
			sleep(1);
			setAngle(90);
			printf("Set angle to 90");
			sleep(1);
			setAngle(0);
			printf("Set angle to 0");
			sleep(1);
			setAngle(90);
			printf("Set angle to 90");
			sleep(1);
			setAngle(180);
			printf("Set angle to 180");
			//! [Interesting]

			printf("exiting application");
		}

		return 0;
	}
}

