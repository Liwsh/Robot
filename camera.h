/*
 * camera.h
 *
 *  Created on: 2016Äê8ÔÂ4ÈÕ
 *      Author: Administrator
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <jpeglib.h>
int camera();

#endif /* CAMERA_H_ */
