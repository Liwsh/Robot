/*
 * servo.h
 *
 *  Created on: 2016年7月28日
 *      Author: 李文胜
 */

#ifndef SERVO_H_
#define SERVO_H_

#define MIN_PULSE_WIDTH             600
#define MAX_PULSE_WIDTH             2500
#define PERIOD                      20000
#define HIGH                        1
#define LOW                         0
#define DEFAULT_WAIT_DISABLE_PWM    0
int m_servoPin;
float m_maxAngle;
int m_currAngle;
int m_minPulseWidth;
int m_maxPulseWidth;
int m_period;
int m_waitAndDisablePwm;
mraa_pwm_context m_pwmServoContext;

#endif /* SERVO_H_ */
