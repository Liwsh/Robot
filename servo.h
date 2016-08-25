/*
 * servo.h
 *
 *  Created on: 2016年8月3日
 *      Author: Administrator
 */

#ifndef SERVO_H_
#define SERVO_H_

#define MIN_PULSE_WIDTH             600
#define MAX_PULSE_WIDTH             2500
#define PERIOD                      20000
#define HIGH                        1
#define LOW                         0
#define DEFAULT_WAIT_DISABLE_PWM    0

float m_maxAngle;
int pin3, pin9;
int m_currAngle;
int m_minPulseWidth;
int m_maxPulseWidth;
int m_period;
int m_waitAndDisablePwm;
mraa_pwm_context m_pwmServoContext;
mraa_pwm_context m_pwmServoContext1;

mraa_result_t haltPwm();
int calcPulseTraveling(int value);
int setAngle(int angle, int pin);
void init(int pin1, int pin2, int minPulseWidth, int maxPulseWidth,int waitAndDisablePwm);
void servo(int angle,int pin);

#endif /* SERVO_H_ */
