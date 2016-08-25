/*
 * capturing from UVC cam
 * requires: libjpeg-dev
 * build: gcc -std=c99 capture.c -ljpeg -o capture
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

/*�������Ķ�д��ʽ������ͷ�������ò���*/
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

/*��ʼ������ͷ*/
void camera_init(camera_t* camera) {
	/*
	 * ��������VIDIOC_QUERYCAP
	 *���ܣ� ��ѯ��Ƶ�豸�Ĺ��� ��
	 *����˵������������ΪV4L2��������������struct v4l2_capability ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0������ִ�гɹ���struct v4l2_capability �ṹ������еķ���
	 *��ǰ��Ƶ�豸��֧�ֵĹ���;����֧����Ƶ������V4L2_CAP_VIDEO_CAPTURE��V4L2_CAP_STREAMING�ȡ�
	 */
	struct v4l2_capability cap;
	if (xioctl(camera->fd, VIDIOC_QUERYCAP, &cap) == -1)
		quit("VIDIOC_QUERYCAP");
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
		quit("no capture");
	if (!(cap.capabilities & V4L2_CAP_STREAMING))
		quit("no streaming");

/*
 *��������VIDIOC_CROPCAP
 *���ܣ� ��ѯ�������޼�����
 *���ܣ� ��Ƶ��׽�豸���Ͻǣ���ȡ������Ŀ�߿���ͨ��VIDIOC_CRAOCAP ���ص�struct v4l2_cropcap�ṹ���е�bound�ӳ�Ա�ṹ�������
 *���Ͻ�Դ���εĿ����ʵ����ȡ��������ͨ��struct v4l2_crop�ṹ�����������ϵͳ��struct v4l2_cropcap��ͬ��
 *Ӧ�ó������ͨ��ʹ��VIDIOC_G_CROP��VIDIOC_S_CROP ioctl����ȡ�����þ���������������ȫ�ڲ�׽��Χ�ڣ���������֮����ܸ���Ӳ
 *�������޸�������ĳߴ缰����λ�á�ÿ����׽�豸����һ��Ĭ�ϵ�Դ���Σ�ͨ��struct v4l2_cropcap�ṹ���е�defrect��Ա�ṹ�������
 *���ε�����Ӧ������Ƶ�ź��еĻ�ԾͼƬ�������Ķ�׼���Һ���������д�������ĵ�����ͼƬ��
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
	 * ��������VIDIOC_S_FMT
	 *���ܣ� ������Ƶ�豸����Ƶ���ݸ�ʽ������������Ƶͼ�����ݵĳ�����ͼ���ʽ��JPEG��YUYV��ʽ����
	 *����˵������������ΪV4L2����Ƶ���ݸ�ʽ����    struct v4l2_format  ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0��
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
	 *��������VIDIOC_REQBUFS
	 *���ܣ� ����V4L2����������Ƶ������������V4L2��Ƶ���������ڴ棩��V4L2����Ƶ�豸�������㣬
	 *λ���ں˿ռ䣬����ͨ��VIDIOC_REQBUFS����������������ڴ�λ���ں˿ռ䣬Ӧ�ó�����ֱ�ӷ��ʣ�
	 *��Ҫͨ������mmap�ڴ�ӳ�亯�����ں˿ռ��ڴ�ӳ�䵽�û��ռ��Ӧ�ó���ͨ�������û��ռ��ַ�������ں˿ռ䡣
	 *����˵������������ΪV4L2�����뻺�������ݽṹ������struct v4l2_requestbuffers  ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0��V4L2��������� ������Ƶ��������
	 *���뻺�壬count�����������
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
	 *��������VIDIOC_QUERYBUF
	 *���ܣ� ��ѯ�Ѿ������V4L2����Ƶ�������������Ϣ��������Ƶ��������ʹ��״̬�����ں˿ռ��ƫ�Ƶ�ַ�����������ȵȡ�
	 *��Ӧ�ó��������ͨ����VIDIOC_QUERYBUF����ȡ�ں˿ռ����Ƶ��������Ϣ��Ȼ����ú���mmap���ں˿ռ��ַӳ��
	 *���û��ռ䣬����Ӧ�ó�����ܹ�����λ���ں˿ռ����Ƶ��������
	 *����˵������������ΪV4L2���������ݽṹ����    struct v4l2_buffer  ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0��struct v4l2_buffer�ṹ������б�����ָ��Ļ������������Ϣ��
	 *һ������£�Ӧ�ó����е���VIDIOC_QUERYBUFȡ�����ں˻�������Ϣ�󣬽����ŵ���mmap�������ں˿ռ��ַӳ�䵽�û��ռ䣬�����û��ռ�Ӧ�ó���ķ��ʡ�
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
	 *��������VIDIOC_QBUF
	 *���ܣ� Ͷ��һ���յ���Ƶ����������Ƶ��������������� ��
	 *����˵������������ΪV4L2���������ݽṹ����    struct v4l2_buffer ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0������ִ�гɹ���ָ��(ָ��)����Ƶ������������Ƶ������У�
	 *����ֵ˵���� ��������Ƶ�豸����ͼ��ʱ����Ӧ����Ƶ���ݱ����浽��Ƶ���������Ӧ����Ƶ�������С�
	 *���뵽�Ļ�������ж�
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
	 *��������VIDIOC_STREAMON
	 *���ܣ� ������Ƶ�ɼ����Ӧ�ó������VIDIOC_STREAMON������Ƶ�ɼ������
	 *��Ƶ�豸��������ʼ�ɼ���Ƶ���ݣ����Ѳɼ�������Ƶ���ݱ��浽��Ƶ��������Ƶ�������С�
	 *����˵������������ΪV4L2����Ƶ���������� enum v4l2_buf_type ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0������ִ�гɹ�����Ƶ�豸��������ʼ�ɼ���Ƶ���ݣ�
	 *��ʱӦ�ó���һ��ͨ������select�������ж�һ֡��Ƶ�����Ƿ�ɼ���ɣ�����Ƶ�豸�������һ֡
	 *��Ƶ���ݲɼ������浽��Ƶ��������ʱ��select�������أ�Ӧ�ó�����ſ��Զ�ȡ��Ƶ���ݣ�����select��������ֱ����Ƶ���ݲɼ���ɡ�
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
	 *��������VIDIOC_DQBUF
	 *���ܣ� ����Ƶ�����������������ȡ��һ���Ѿ�������һ֡��Ƶ���ݵ���Ƶ��������
	 *����˵������������ΪV4L2���������ݽṹ����    struct v4l2_buffer ��
	 *����ֵ˵���� ִ�гɹ�ʱ����������ֵΪ 0����Ӧ���ں���Ƶ�������б����е�ǰ���㵽����Ƶ���ݣ�Ӧ�ó������ͨ�������û��ռ�����ȡ����Ƶ���ݡ�
	 *ǰ���Ѿ�ͨ�����ú���mmap�����û��ռ���ں˿ռ���ڴ�ӳ�䡣
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

/*yuyv��ʽתrgb��ʽ
 *ԭʼ�������������� Y0 U0 V0 ,Y1 U1 V1,Y2 U2 V2,Y3 U3 V3
 *����4��2��2���������ݱ���� Y0 U0 ,Y1 V1 ,Y2 U2,Y3 V3
 *�����ԭ����ΪĳһЩ���ݶ�ʧ�Ͳ��� Y0 U0 V1,Y1 U0 V1,Y2 U2 V3 ,Y3 U3 Y2
 * R = Y + 1.4075 *��V-128��
 * G = Y �C 0.3455 *��U �C128�� �C 0.7169 *��V �C128��
 * B = Y + 1.779 *��U �C 128��
 * �����������㣬�ӿ������ٶȣ�y������ϵ��������256
 * һ��Y����һ�����أ���һ��Y��UV�����������һ�����أ����Ե�0������Y0�͵�һ������Y1���ǹ��õ�0�����ص�U0��V0��
 * ����Y0U0Y1V0�൱���������أ�ռ����4���ֽڵĴ洢�ռ䣬ƽ��һ������ռ�������ֽڡ�
 * RGBɫ��ģʽһ�������������������ɣ���һ������ռ�������ֽڡ�
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
	/*���豸�����ò���*/
	camera_t* camera = camera_open("/dev/video0", 640, 480);

	camera_init(camera);

	camera_start(camera);
	int i;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	/* skip 5 frames for booting a cam */
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
