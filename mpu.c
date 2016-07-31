#include <stdio.h>
#include <string.h>
#include <mraa.h>
#include <math.h>
#include <sys/time.h>
float timeChange=20;//�˲�������ʱ��������
float dt=0.02;//ע�⣺dt��ȡֵΪ�˲�������ʱ��
// ������
float angleAx,gyroGy;//�����ĽǶȣ���x��нǣ��ͽ��ٶ�
int16_t ax, ay, az, gx, gy, gz;//������ԭʼ���� 3�����ٶ�+3�����ٶ�
//һ���˲�
float K1 =0.05; // �Լ��ٶȼ�ȡֵ��Ȩ��
//float dt=20*0.001;//ע�⣺dt��ȡֵΪ�˲�������ʱ��
float angle1;//һ���˲��Ƕ����
//�����˲�
float K2 =0.2; // �Լ��ٶȼ�ȡֵ��Ȩ��
float x1,x2,y;//�����м����
//float dt=20*0.001;//ע�⣺dt��ȡֵΪ�˲�������ʱ��
float angle2;//er���˲��Ƕ����
//�������˲������뺯��
float angle, angle_dot;//�ǶȺͽ��ٶ�
float angle_0, angle_dot_0;//�ɼ����ĽǶȺͽ��ٶ�
//float dt=20*0.001;//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��
//һ��Ϊ�����м����
uint8_t buffer[14];
mraa_i2c_context i2c;

float P[2][2] = {{ 1, 0 },
              { 0, 1 }};
float Pdot[4] ={ 0,0,0,0};
float Q_angle=0.001, Q_gyro=0.005; //�Ƕ��������Ŷ�,���ٶ��������Ŷ�
float R_angle=0.5 ,C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

#define MPU6050_DEFAULT_ADDRESS 0x68
#define MPU6050_RA_PWR_MGMT_1	0x6B
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_GYRO_CONFIG	0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_CLOCK_PLL_XGYRO 0x01
#define MPU6050_GYRO_FS_250   	0x00
#define MPU6050_ACCEL_FS_2		0x00
#define PI 3.1415926535897932384626433832795
uint8_t devAddr;
#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2
#define MAX_NUMBER_OF_EVENTS 10
#define NO_TIMER_AVAILABLE   -1

/*
 * �ṹ�����ڻ�ȡʱ��
 */
struct timeval tv1, tv2;
struct timezone tz;

/*
 * �Զ���ṹ�����ڻ�ȡ��ʱѭ�����¼���
 */
struct EVENT {
	void (*update)(unsigned long now, int i);
	int8_t eventType; /*�¼�״̬*/
	unsigned long period;/*�¼�ִ��ʱ�䳤�ȣ���λms(����)*/
	int repeatCount; /*�¼�ѭ��״̬*/
	uint8_t pin; /*���ź�*/
	uint8_t pinState;/*����״̬*/
	void (*callback)(void);/*ִ�е��¼�*/
	unsigned long lastEventTime;/*�ϴ�ִ���¼���ʱ��*/
	int count;/*����*/
} events[MAX_NUMBER_OF_EVENTS];
void timeupdate(void);
void update(unsigned long now);
void costupdate(unsigned long now, int i);
int8_t findFreeEventIndex(void);
int8_t every(unsigned long period, void (*callback)(), int repeatCount);




void yijielvbo(float angle_m, float gyro_m)
{
    angle1 = K1 * angle_m+ (1-K1) * (angle1 + gyro_m * dt);
}

void erjielvbo(float angle_m,float gyro_m)
{
    x1=(angle_m-angle2)*(1-K2)*(1-K2);
    y=y+x1*dt;
    x2=y+2*(1-K2)*(angle_m-angle2)+gyro_m;
    angle2=angle2+ x2*dt;
}

void kalman_Filter(double angle_m,double gyro_m)
{
angle+=(gyro_m-q_bias) * dt;
angle_err = angle_m - angle;
Pdot[0]=Q_angle - P[0][1] - P[1][0];
Pdot[1]=- P[1][1];
Pdot[2]=- P[1][1];
Pdot[3]=Q_gyro;
P[0][0] += Pdot[0] * dt;
P[0][1] += Pdot[1] * dt;
P[1][0] += Pdot[2] * dt;
P[1][1] += Pdot[3] * dt;
PCt_0 = C_0 * P[0][0];
PCt_1 = C_0 * P[1][0];
E = R_angle + C_0 * PCt_0;
K_0 = PCt_0 / E;
K_1 = PCt_1 / E;
t_0 = PCt_0;
t_1 = C_0 * P[0][1];
P[0][0] -= K_0 * t_0;
P[0][1] -= K_0 * t_1;
P[1][0] -= K_1 * t_0;
P[1][1] -= K_1 * t_1;
angle += K_0 * angle_err; //���ŽǶ�
q_bias += K_1 * angle_err;
angle_dot = gyro_m-q_bias;//���Ž��ٶ�
}
void getangle()
{
	mraa_i2c_read_bytes_data(i2c,MPU6050_RA_ACCEL_XOUT_H,buffer,14);
		 ax = (((int16_t)buffer[0]) << 8) | buffer[1];
		 ay = (((int16_t)buffer[2]) << 8) | buffer[3];
		 az = (((int16_t)buffer[4]) << 8) | buffer[5];
		 gx = (((int16_t)buffer[8]) << 8) | buffer[9];
		 gy = (((int16_t)buffer[10]) << 8) | buffer[11];
		 gz = (((int16_t)buffer[12]) << 8) | buffer[13];
//    angleAx=atan2(ax,az)*180/PI;//������x��н�
//    gyroGy=-gy/131.00;//������ٶ�
    angleAx=atan2(ay,az)*180/PI;//������y��н�
    gyroGy=-gx/131.00;//
    yijielvbo(angleAx,gyroGy);//һ���˲�
    erjielvbo(angleAx,gyroGy);//�����˲�
    kalman_Filter(angleAx,gyroGy);   //�������˲�
}
void timeupdate(void) {
	gettimeofday(&tv2, &tz);
	unsigned long now = tv2.tv_sec*1000 + tv2.tv_usec /1000;
	update(now);
}

/*
 *�¼�����
 */
void update(unsigned long now) {
	int8_t i;
	for (i = 0; i < MAX_NUMBER_OF_EVENTS; i++) {
		if (events[i].eventType != EVENT_NONE) {
			events[i].update = costupdate;
			events[i].update(now, i);
		}
	}
}

/*
 *ѭ��ʱ�����
 */
void costupdate(unsigned long now, int i) {
	if (now - events[i].lastEventTime >= events[i].period) {
		switch (events[i].eventType) {
		case EVENT_EVERY:
			events[i].callback();
			break;

		case EVENT_OSCILLATE:
			events[i].pinState = !events[i].pinState;
//			Write(events[i].pin, events[i].pinState);
			break;
		}
		events[i].lastEventTime = now;
		events[i].count++;
	}
	if (events[i].repeatCount > -1
			&& events[i].count >= events[i].repeatCount) {
		events[i].eventType = EVENT_NONE;
	}
}

/*
 *��ȡѭ����ʱ�¼���
 */
int8_t findFreeEventIndex(void) {
	int8_t i;
	for (i = 0; i < MAX_NUMBER_OF_EVENTS; i++) {
		if (events[i].eventType == EVENT_NONE) {
			printf("i is %d\n", i);
			return i;
		}
	}
	return NO_TIMER_AVAILABLE;
}

/*
 *����ѭ���¼�
 */
int8_t every(unsigned long period, void (*callback)(), int repeatCount) {
	int8_t i = findFreeEventIndex();
	if (i == -1)
		return -1;
	events[i].eventType = EVENT_EVERY;
	events[i].period = period;
	events[i].repeatCount = repeatCount;
	events[i].callback = callback;
	gettimeofday(&tv1, &tz);
	events[i].lastEventTime = tv1.tv_sec*1000 + tv1.tv_usec / 1000;
	events[i].count = 0;
	return i;
}
void print(){
	usleep(10000);
	printf("angleAx=%f, ", angleAx);
	printf("angle1=%f, ", angle1);
	printf("angle2=%f, ", angle2); //���������
	printf("angle=%f, \n", angle); //���������
}
void main(){
	devAddr = MPU6050_DEFAULT_ADDRESS;
	i2c = mraa_i2c_init(0);
	/** Power on and prepare for general usage.
	 * This will activate the device and take it out of sleep mode (which must be done
	 * after start-up). This function also sets both the accelerometer and the gyroscope
	 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
	 * the clock source to use the X Gyro for reference, which is slightly better than
	 * the default internal clock source.
	 */
	mraa_i2c_address(i2c,devAddr);
	mraa_i2c_write_byte_data(i2c,MPU6050_CLOCK_PLL_XGYRO,MPU6050_RA_PWR_MGMT_1);
	mraa_i2c_write_byte_data(i2c,MPU6050_RA_GYRO_CONFIG,MPU6050_GYRO_FS_250);
	mraa_i2c_write_byte_data(i2c,MPU6050_RA_ACCEL_CONFIG,MPU6050_ACCEL_FS_2);
	int ev1=every(20,getangle,-1);
	int ev2=every(50,print,-1);
while(1){
	timeupdate();
}
}
