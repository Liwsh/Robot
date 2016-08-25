/*
 * capturing from UVC cam
 * requires: libjpeg-dev
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "camera.h"

void quit(const char * msg) {
	fprintf(stderr, "[%s] %d: %s\n", msg, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void* arg) {
	int i;
	for (i = 0; i < 100; i++) {
		int r = ioctl(fd, request, arg);
		if (r != -1 || errno != EINTR)
			return r;
	}
	return -1;
}

typedef struct {
	uint8_t* start;
	size_t length;
} buffer_t;

typedef struct {
	int fd;
	uint32_t width;
	uint32_t height;
	size_t buffer_count;
	buffer_t* buffers;
	buffer_t head;
} camera_t;

/*非阻塞的读写方式打开摄像头，并设置参数*/
camera_t* camera_open(const char * device, uint32_t width, uint32_t height) {
	int fd = open(device, O_RDWR | O_NONBLOCK, 0);
	if (fd == -1)
		quit("open");
	camera_t* camera = malloc(sizeof(camera_t));
	camera->fd = fd;
	camera->width = width;
	camera->height = height;
	camera->buffer_count = 0;
	camera->buffers = NULL;
	camera->head.length = 0;
	camera->head.start = NULL;
	return camera;
}

/*初始化摄像头*/
void camera_init(camera_t* camera) {
	/*
	 * 控制命令VIDIOC_QUERYCAP
	 *功能： 查询视频设备的功能 ；
	 *参数说明：参数类型为V4L2的能力描述类型struct v4l2_capability ；
	 *返回值说明： 执行成功时，函数返回值为 0；函数执行成功后，struct v4l2_capability 结构体变量中的返回
	 *当前视频设备所支持的功能;例如支持视频捕获功能V4L2_CAP_VIDEO_CAPTURE、V4L2_CAP_STREAMING等。
	 */
	struct v4l2_capability cap;
	if (xioctl(camera->fd, VIDIOC_QUERYCAP, &cap) == -1)
		quit("VIDIOC_QUERYCAP");
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
		quit("no capture");
	if (!(cap.capabilities & V4L2_CAP_STREAMING))
		quit("no streaming");

/*
 *控制命令VIDIOC_CROPCAP
 *功能： 查询驱动的修剪能力
 *功能： 视频捕捉设备左上角，可取样区域的宽高可以通过VIDIOC_CRAOCAP 返回的struct v4l2_cropcap结构体中的bound子成员结构体给出。
 *左上角源矩形的宽高是实际能取样的区域，通过struct v4l2_crop结构体给出，坐标系统与struct v4l2_cropcap相同。
 *应用程序可以通过使用VIDIOC_G_CROP和VIDIOC_S_CROP ioctl来获取和设置矩形区域。它必须完全在捕捉范围内，而且驱动之后可能根据硬
 *件限制修改所请求的尺寸及（或）位置。每个捕捉设备都有一个默认的源矩形，通过struct v4l2_cropcap结构提中的defrect成员结构体给出。
 *矩形的中心应该与视频信号中的活跃图片区域中心对准，且涵盖驱动编写者所关心的完整图片。
*/
	struct v4l2_cropcap cropcap;
	memset(&cropcap, 0, sizeof cropcap);
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(camera->fd, VIDIOC_CROPCAP, &cropcap) == 0) {
		struct v4l2_crop crop;
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;
		if (xioctl(camera->fd, VIDIOC_S_CROP, &crop) == -1) {
			// cropping not supported
		}
	}

	/*
	 * 控制命令VIDIOC_S_FMT
	 *功能： 设置视频设备的视频数据格式，例如设置视频图像数据的长、宽，图像格式（JPEG、YUYV格式）；
	 *参数说明：参数类型为V4L2的视频数据格式类型    struct v4l2_format  ；
	 *返回值说明： 执行成功时，函数返回值为 0；
	 */
	struct v4l2_format format;
	memset(&format, 0, sizeof format);
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = camera->width;
	format.fmt.pix.height = camera->height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	if (xioctl(camera->fd, VIDIOC_S_FMT, &format) == -1)
		quit("VIDIOC_S_FMT");

	/*
	 *控制命令VIDIOC_REQBUFS
	 *功能： 请求V4L2驱动分配视频缓冲区（申请V4L2视频驱动分配内存），V4L2是视频设备的驱动层，
	 *位于内核空间，所以通过VIDIOC_REQBUFS控制命令字申请的内存位于内核空间，应用程序不能直接访问，
	 *需要通过调用mmap内存映射函数把内核空间内存映射到用户空间后，应用程序通过访问用户空间地址来访问内核空间。
	 *参数说明：参数类型为V4L2的申请缓冲区数据结构体类型struct v4l2_requestbuffers  ；
	 *返回值说明： 执行成功时，函数返回值为 0；V4L2驱动层分配 好了视频缓冲区；
	 *申请缓冲，count是申请的数量
	 */
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof req);
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (xioctl(camera->fd, VIDIOC_REQBUFS, &req) == -1)
		quit("VIDIOC_REQBUFS");
	camera->buffer_count = req.count;
	camera->buffers = calloc(req.count, sizeof(buffer_t));

	/*
	 *控制命令VIDIOC_QUERYBUF
	 *功能： 查询已经分配的V4L2的视频缓冲区的相关信息，包括视频缓冲区的使用状态、在内核空间的偏移地址、缓冲区长度等。
	 *在应用程序设计中通过调VIDIOC_QUERYBUF来获取内核空间的视频缓冲区信息，然后调用函数mmap把内核空间地址映射
	 *到用户空间，这样应用程序才能够访问位于内核空间的视频缓冲区。
	 *参数说明：参数类型为V4L2缓冲区数据结构类型    struct v4l2_buffer  ；
	 *返回值说明： 执行成功时，函数返回值为 0；struct v4l2_buffer结构体变量中保存了指令的缓冲区的相关信息；
	 *一般情况下，应用程序中调用VIDIOC_QUERYBUF取得了内核缓冲区信息后，紧接着调用mmap函数把内核空间地址映射到用户空间，方便用户空间应用程序的访问。
	 */
	size_t buf_max = 0;
	size_t i;
	for (i = 0; i < camera->buffer_count; i++) {
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (xioctl(camera->fd, VIDIOC_QUERYBUF, &buf) == -1)
			quit("VIDIOC_QUERYBUF");
		if (buf.length > buf_max)
			buf_max = buf.length;
		camera->buffers[i].length = buf.length;
		camera->buffers[i].start = mmap(NULL, buf.length,
		PROT_READ | PROT_WRITE, MAP_SHARED, camera->fd, buf.m.offset);
		if (camera->buffers[i].start == MAP_FAILED)
			quit("mmap");
	}
	camera->head.start = malloc(buf_max);
}

void camera_start(camera_t* camera) {
	/*
	 *控制命令VIDIOC_QBUF
	 *功能： 投放一个空的视频缓冲区到视频缓冲区输入队列中 ；
	 *参数说明：参数类型为V4L2缓冲区数据结构类型    struct v4l2_buffer ；
	 *返回值说明： 执行成功时，函数返回值为 0；函数执行成功后，指令(指定)的视频缓冲区进入视频输入队列，
	 *返回值说明： 在启动视频设备拍摄图像时，相应的视频数据被保存到视频输入队列相应的视频缓冲区中。
	 *申请到的缓冲进入列队
	 */
	size_t i;
	for (i = 0; i < camera->buffer_count; i++) {
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1)
			quit("VIDIOC_QBUF");
	}

	/*
	 *控制命令VIDIOC_STREAMON
	 *功能： 启动视频采集命令，应用程序调用VIDIOC_STREAMON启动视频采集命令后，
	 *视频设备驱动程序开始采集视频数据，并把采集到的视频数据保存到视频驱动的视频缓冲区中。
	 *参数说明：参数类型为V4L2的视频缓冲区类型 enum v4l2_buf_type ；
	 *返回值说明： 执行成功时，函数返回值为 0；函数执行成功后，视频设备驱动程序开始采集视频数据，
	 *此时应用程序一般通过调用select函数来判断一帧视频数据是否采集完成，当视频设备驱动完成一帧
	 *视频数据采集并保存到视频缓冲区中时，select函数返回，应用程序接着可以读取视频数据；否则select函数阻塞直到视频数据采集完成。
	 */
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(camera->fd, VIDIOC_STREAMON, &type) == -1)
		quit("VIDIOC_STREAMON");
}

void camera_stop(camera_t* camera) {
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(camera->fd, VIDIOC_STREAMOFF, &type) == -1)
		quit("VIDIOC_STREAMOFF");
}

void camera_finish(camera_t* camera) {
	size_t i;
	for (i = 0; i < camera->buffer_count; i++) {
		munmap(camera->buffers[i].start, camera->buffers[i].length);
	}
	free(camera->buffers);
	camera->buffer_count = 0;
	camera->buffers = NULL;
	free(camera->head.start);
	camera->head.length = 0;
	camera->head.start = NULL;
}

void camera_close(camera_t* camera) {
	if (close(camera->fd) == -1)
		quit("close");
	free(camera);
}

int camera_capture(camera_t* camera) {
	/*
	 *控制命令VIDIOC_DQBUF
	 *功能： 从视频缓冲区的输出队列中取得一个已经保存有一帧视频数据的视频缓冲区；
	 *参数说明：参数类型为V4L2缓冲区数据结构类型    struct v4l2_buffer ；
	 *返回值说明： 执行成功时，函数返回值为 0，相应的内核视频缓冲区中保存有当前拍摄到的视频数据，应用程序可以通过访问用户空间来读取该视频数据。
	 *前面已经通过调用函数mmap做了用户空间和内核空间的内存映射。
	 */
	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if (xioctl(camera->fd, VIDIOC_DQBUF, &buf) == -1)
		return FALSE;
	memcpy(camera->head.start, camera->buffers[buf.index].start, buf.bytesused);
	camera->head.length = buf.bytesused;
	if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1)
		return FALSE;
	return TRUE;
}

int camera_frame(camera_t* camera, struct timeval timeout) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(camera->fd, &fds);
	int r = select(camera->fd + 1, &fds, 0, 0, &timeout);
	if (r == -1)
		quit("select");
	if (r == 0)
		return FALSE;
	return camera_capture(camera);
}

void jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height,
		int quality) {
	JSAMPARRAY image;
	size_t i, j;
	image = calloc(height, sizeof(JSAMPROW));
	for (i = 0; i < height; i++) {
		image[i] = calloc(width * 3, sizeof(JSAMPLE));
		for (j = 0; j < width; j++) {
			image[i][j * 3 + 0] = rgb[(i * width + j) * 3 + 0];
			image[i][j * 3 + 1] = rgb[(i * width + j) * 3 + 1];
			image[i][j * 3 + 2] = rgb[(i * width + j) * 3 + 2];
		}
	}

	struct jpeg_compress_struct compress;
	struct jpeg_error_mgr error;
	compress.err = jpeg_std_error(&error);
	jpeg_create_compress(&compress);
	jpeg_stdio_dest(&compress, dest);

	compress.image_width = width;
	compress.image_height = height;
	compress.input_components = 3;
	compress.in_color_space = JCS_RGB;
	jpeg_set_defaults(&compress);
	jpeg_set_quality(&compress, quality, TRUE);
	jpeg_start_compress(&compress, TRUE);
	jpeg_write_scanlines(&compress, image, height);
	jpeg_finish_compress(&compress);
	jpeg_destroy_compress(&compress);

	for (i = 0; i < height; i++) {
		free(image[i]);
	}
	free(image);
}

int minmax(int min, int v, int max) {
	return (v < min) ? min : (max < v) ? max : v;
}

/*yuyv格式转rgb格式
 *原始数据三个像素是 Y0 U0 V0 ,Y1 U1 V1,Y2 U2 V2,Y3 U3 V3
 *经过4：2：2采样后，数据变成了 Y0 U0 ,Y1 V1 ,Y2 U2,Y3 V3
 *如果还原后，因为某一些数据丢失就补成 Y0 U0 V1,Y1 U0 V1,Y2 U2 V3 ,Y3 U3 Y2
 * R = Y + 1.4075 *（V-128）
 * G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
 * B = Y + 1.779 *（U – 128）
 * 采用整形运算，加快运算速度，y分量和系数都扩大256
 * 一个Y代表一个像素，而一个Y和UV组合起来构成一个像素，所以第0个像素Y0和第一个像素Y1都是共用第0个像素的U0和V0。
 * 所以Y0U0Y1V0相当于两个像素，占用了4个字节的存储空间，平均一个像素占用两个字节。
 * RGB色彩模式一个像素由三个分量构成，即一个像素占用三个字节。
 */

uint8_t* yuyv2rgb(uint8_t* yuyv, uint32_t width, uint32_t height) {
	size_t i, j;
	uint8_t* rgb = calloc(width * height * 3, sizeof(uint8_t));
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 2) {
			size_t index = i * width + j;
			int y0 = yuyv[index * 2 + 0] << 8;
			int u = yuyv[index * 2 + 1] - 128;
			int y1 = yuyv[index * 2 + 2] << 8;
			int v = yuyv[index * 2 + 3] - 128;
			rgb[index * 3 + 0] = minmax(0, (y0 + 359 * v) >> 8, 255);
			rgb[index * 3 + 1] = minmax(0, (y0 + 88 * v - 183 * u) >> 8, 255);
			rgb[index * 3 + 2] = minmax(0, (y0 + 454 * u) >> 8, 255);
			rgb[index * 3 + 3] = minmax(0, (y1 + 359 * v) >> 8, 255);
			rgb[index * 3 + 4] = minmax(0, (y1 + 88 * v - 183 * u) >> 8, 255);
			rgb[index * 3 + 5] = minmax(0, (y1 + 454 * u) >> 8, 255);
		}
	}
	return rgb;
}

int camera() {
	/*打开设备并设置参数*/
	camera_t* camera = camera_open("/dev/video0", 640, 480);

	camera_init(camera);

	camera_start(camera);
	int i;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	for (i = 0; i < 5; i++) {
		camera_frame(camera, timeout);
	}
	camera_frame(camera, timeout);

	unsigned char* rgb = yuyv2rgb(camera->head.start, camera->width,
			camera->height);
	FILE* out = fopen("result.jpg", "w");
	jpeg(out, rgb, camera->width, camera->height, 100);
	fclose(out);
	free(rgb);

	camera_stop(camera);
	camera_finish(camera);
	camera_close(camera);
	system("python facedetect.py result.jpg");
	return 0;
}
