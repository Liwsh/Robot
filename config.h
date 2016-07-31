/*
 * config.h
 *
 *  Created on: 2016年7月7日
 *      Author: Liwensheng
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "mraa.h"
#define MOTOR_INIT     mraa_gpio_context motor1 =mraa_gpio_init(7);mraa_gpio_context motor2 =mraa_gpio_init(8);mraa_gpio_context motor3 =mraa_gpio_init(12);mraa_gpio_context motor4 =mraa_gpio_init(13);
#define MOTOR_MODE     mraa_gpio_dir(motor1, MRAA_GPIO_OUT);mraa_gpio_dir(motor2, MRAA_GPIO_OUT);mraa_gpio_dir(motor3, MRAA_GPIO_OUT);mraa_gpio_dir(motor4, MRAA_GPIO_OUT);
#define MOTOR_FORWARD  mraa_gpio_write(motor1, 1);mraa_gpio_write(motor3, 1);mraa_gpio_write(motor2, 0);mraa_gpio_write(motor4, 0);
#define MOTOR_BACK     mraa_gpio_write(motor1, 0);mraa_gpio_write(motor3, 0);mraa_gpio_write(motor2, 1);mraa_gpio_write(motor4, 1);
#define MOTOR_RIGHT    mraa_gpio_write(motor1, 0);mraa_gpio_write(motor3, 1);mraa_gpio_write(motor2, 1);mraa_gpio_write(motor4, 0);
#define MOTOR_LEFT     mraa_gpio_write(motor1, 1);mraa_gpio_write(motor3, 0);mraa_gpio_write(motor2, 0);mraa_gpio_write(motor4, 1);
#define MOTOR_STOP     mraa_gpio_write(motor1, 0);mraa_gpio_write(motor3, 0);mraa_gpio_write(motor2, 0);mraa_gpio_write(motor4, 0);

unsigned char buffer[3];	/*数据接收数组*/

#endif /* CONFIG_H_ */
