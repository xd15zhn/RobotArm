#include "solve.h"
#include <math.h>
#define ABS(x)                   ((x)>=0?(x):-(x))
#define LIMIT(x,min,max)         (((x)<=(min) ? (min) : ((x)>=(max) ? (max) : (x))))
/*****�ļ�˵��*********
��е�۵������˶�ѧ�͹켣��ֵ
�̶�����ָ�̶��ڼ�ؽڵ�����,���������������һ����z���������ƽ��
����Inverse_Solve()�����к��й̶�����֮��,�������������ᵽ�������Ϊ��������
�������ᵽ�ĽǶȾ�Ϊrad�Ƕ�
**********************/

constexpr double PI = 3.14159265358979323846;
constexpr double L1 = 93;  //�ؽ�1����
constexpr double L2 = 80;  //�ؽ�2����
constexpr double L3 = 81;  //�ؽ�3����
constexpr double L4 = 172;  //�ؽ�4����
constexpr double DIST = 100;  //��ֵ�����

/**********************
���˶�ѧ�����ؽڽǶ�
* theta:����,���ؽ�DH����
* position:���,Ŀ�������
**********************/
void Forward_Solve(const double theta[], double position[])
{
	double x = 81 + 172 * sin(theta[3]);
	double y = -172 * cos(theta[3]);
	double z = 0;
	double temp[3];
	temp[0] = x * cos(theta[2]) - y * sin(theta[2]) + 80;
	temp[1] = x * sin(theta[2]) + y * cos(theta[2]);
	temp[2] = z;
	x = temp[0]; y = temp[1]; z = temp[2];
	temp[0] = x * cos(theta[1]) - y * sin(theta[1]);
	temp[1] = -z;
	temp[2] = x * sin(theta[1]) + y * cos(theta[1]);
	x = temp[0]; y = temp[1]; z = temp[2];
	temp[0] = x * cos(theta[0]) - y * sin(theta[0]);
	temp[1] = x * sin(theta[0]) + y * cos(theta[0]);
	temp[2] = z + 93;
	position[0] = temp[0];
	position[1] = temp[1];
	position[2] = temp[2];
}

/**********************
���˶�ѧ�����ؽڽǶ�
�����ر�˵��Ϊ��������֮��,�ú����е������Ϊ�̶�����
* position:����,Ŀ������������
* theta:���븴λʱ��DH����,��������ؽڲ���
* return:����Ƿ����.0,����;����,������
**********************/
int Inverse_Solve(const double position[], double theta[])
{
	double x = position[0];
	double y = position[1];
	double z = position[2] - L1;
	double r = x * x + y * y + z * z;
	double ResetJoint[4];
	double pitch = 0.78;  //ĩ��ִ��������ؽ������Ŀ��������
	double targetPitch;  //ĩ��ִ��������ؽ������ԭ�������
	double costheta;
	for (int i = 0; i < 4; i++)
		ResetJoint[i] = theta[i];
	if (r > (L2 + L3 + L4) * (L2 + L3 + L4))  //Ŀ��㳬����������
		return -1;
	theta[0] = atan2(y, x);
	if (ABS(theta[0] - ResetJoint[0]) >= 0.7854)  //Ŀ��㷽λ�ǳ���������Χ
		return -2;
	while (1) {
		z = position[2] - L1 + L4 * sin(pitch);  //��ؽ�z����
		r = L4 * cos(pitch);  //��ؽ���xoyƽ���ͶӰ��ԭ��ľ���
		x = position[0] - r * cos(theta[0]);  //��ؽ�x����
		y = position[1] - r * sin(theta[0]);  //��ؽ�y����
		r = sqrt(x * x + y * y + z * z);  //��ؽ���ԭ��ľ���
		costheta = (L2 * L2 + L3 * L3 - r * r) / L2 / L3 / 2.0;  //��L2��L3�ļн�
		if ((costheta >= -1) && (costheta <= 1)) {  //�н���Ч
			theta[2] = -acos(-costheta);  //ȡ������Ϊ�ؽڽǶ�,�ڶ��ʱȡ�ⲿ���Ͻ�
			if (ABS(theta[2] - ResetJoint[2]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
			targetPitch = atan2(z, sqrt(x * x + y * y));  //���ⲿ�ؽ������ԭ�������
			costheta = (L2 * L2 + r * r - L3 * L3) / L2 / r / 2.0;  //��L2���Ǻ��ⲿ�ؽ�����֮��
			theta[1] = targetPitch + acos(costheta);  //��L2����
			if (ABS(theta[1] - ResetJoint[1]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
			theta[3] = PI / 2.0 - pitch - theta[1] - theta[2];  //L4�����L3������
			if (ABS(theta[3] - ResetJoint[3]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
			return 0;
		}
	PITCHADD:
		pitch += (pitch >= 0.78) ? 0.01 : -0.01;
		if (pitch > 1.5708)
			pitch = 0.77;
		else if (pitch < -1.5708) {  //�޿��н�
			theta[1] = ResetJoint[1];
			theta[2] = ResetJoint[2];
			theta[3] = ResetJoint[3];
			return -3;
		}
	}
}

/**********************
��е�۹켣�滮
pos/theta           |x        |y        |z		  | |��λ��     |��ؽ�     |��ؽ�     |��ؽ�
��ʼλ��			|pos[0][0]|pos[0][1]|pos[0][2]| |theta[0][0]|theta[0][1]|theta[0][2]|theta[0][3]
������ʼλ�õĲ�ֵ��|         |         |pos[1][2]| |           |           |           |theta[1][3]
������ֹλ�õĲ�ֵ��|         |         |pos[2][2]| |           |           |           |theta[2][3]
��ֹλ��			|pos[3][0]|         |pos[3][2]| |theta[3][0]|           |           |theta[3][3]
**********************/
struct Trajectory
{
	double pos[4][3];
	double theta[4][4];
} *mcu61;

/**********************
Ѱ�Ҳ�ֵ��.��ֵ���Ŀ���ķ�λ����ͬ,������Ϊ��ֵ
* position:����,Ŀ�������
* AngleIns:���,��ֵ�������Ӧ�ĹؽڽǶ�
* return:��ֵ���Ƿ����.0,����;����,������
**********************/
int Trajectory_Interpolation(const double* position, double* AngleIns)
{
	double azimuth = atan2(position[1], position[0]);  //��λ��
	double PosIns[3];  //��ֵ������
	for (double pitch = 0; pitch <= 3.14; pitch += 0.01)
	{
		PosIns[2] = position[2] + DIST * sin(pitch);
		PosIns[0] = position[0] - DIST * cos(pitch) * cos(azimuth);
		PosIns[1] = position[1] - DIST * cos(pitch) * sin(azimuth);
		if (Inverse_Solve(PosIns, AngleIns) == 0)
			return 0;
	}
	return -1;
}

/**********************
�켣�滮��ʼ��
* ResetAngle:����,����ʱ��DH�����Ƕ�
* StartAngle:����,��ʼλ�õĸ��ؽ�DH�����Ƕ�
* EndPosition:����,��ֹλ��
* return:�켣�滮�Ƿ���ڽ�.0,����;����,������
**********************/
int Trajectory_Init(const double* ResetAngle, const double* StartAngle, const double* EndPosition)
{
	mcu61 = new Trajectory;
	double StartPosition[3];
	mcu61->theta[0][0] = StartAngle[0];
	mcu61->theta[0][1] = StartAngle[1];
	mcu61->theta[0][2] = StartAngle[2];
	mcu61->theta[0][3] = StartAngle[3];
	mcu61->theta[1][0] = mcu61->theta[2][0] = mcu61->theta[3][0] = ResetAngle[0];
	mcu61->theta[1][1] = mcu61->theta[2][1] = mcu61->theta[3][1] = ResetAngle[1];
	mcu61->theta[1][2] = mcu61->theta[2][2] = mcu61->theta[3][2] = ResetAngle[2];
	mcu61->theta[1][3] = mcu61->theta[2][3] = mcu61->theta[3][3] = ResetAngle[3];
	Forward_Solve(StartAngle, StartPosition);
	mcu61->pos[0][0] = StartPosition[0];
	mcu61->pos[0][1] = StartPosition[1];
	mcu61->pos[0][2] = StartPosition[2];
	mcu61->pos[3][0] = EndPosition[0];
	mcu61->pos[3][1] = EndPosition[1];
	mcu61->pos[3][2] = EndPosition[2];
	if (Trajectory_Interpolation(mcu61->pos[0], mcu61->theta[1]) != 0)
		return 1;
	if (Trajectory_Interpolation(mcu61->pos[3], mcu61->theta[2]) != 0)
		return 2;
	if (Inverse_Solve(EndPosition, mcu61->theta[3]) != 0)
		return 3;
	return 0;
}

/**********************
�����ֵ���ؽ�DH�����Ƕ�
* t:����,ʱ��
* angle:���,���ؽ�DH�����Ƕ�
* return:1,�ͷ�;2,ץ��;3,ץȡ����;0,����
**********************/
int Trajectory_Output(int t, double* angle)
{
	int i = t / 40;
	i = LIMIT(i, 0, 3);
	angle[0] = mcu61->theta[i][0];
	angle[1] = mcu61->theta[i][1];
	angle[2] = mcu61->theta[i][2];
	angle[3] = mcu61->theta[i][3];
	if (t < 120)
		return 1;
	else if (t < 160)
		return 2;
	else if (t >= 160)
		return 3;
	else
		return 0;
}

/**********************
�켣�滮����
**********************/
void Trajectory_Finish(void)
{
	if (mcu61 != NULL)
		delete mcu61;
}