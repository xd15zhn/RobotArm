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
constexpr double L1 = 93;  //����1����
constexpr double L2 = 80;  //����2����
constexpr double L3 = 81;  //����3����
constexpr double L4 = 195;  //����4����

/**********************
���˶�ѧ�����ؽڽǶ�
* theta:����,���ؽ�DH����
* position:���,Ŀ�������
**********************/
void Forward_Solve(const double theta[], double position[])
{
	double x = L3 + L4 * sin(theta[3]);
	double y = -L4 * cos(theta[3]);
	double z = 0;
	double temp[3];
	temp[0] = x * cos(theta[2]) - y * sin(theta[2]) + L2;
	temp[1] = x * sin(theta[2]) + y * cos(theta[2]);
	temp[2] = z;
	x = temp[0]; y = temp[1]; z = temp[2];
	temp[0] = x * cos(theta[1]) - y * sin(theta[1]);
	temp[1] = -z;
	temp[2] = x * sin(theta[1]) + y * cos(theta[1]);
	x = temp[0]; y = temp[1]; z = temp[2];
	temp[0] = x * cos(theta[0]) - y * sin(theta[0]);
	temp[1] = x * sin(theta[0]) + y * cos(theta[0]);
	temp[2] = z + L1;
	position[0] = temp[0];
	position[1] = temp[1];
	position[2] = temp[2];
}

/**********************
���˶�ѧ�����ؽڽǶ�
�����ر�˵��Ϊ��������֮��,�ú����е������Ϊ�̶�����
���ؽڽǶȷ�����:̧ͷΪ��,������ʱ��Ϊ��
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
		return 1;
	theta[0] = atan2(y, x);  //��λ��
	if (ABS(theta[0] - ResetJoint[0]) >= 0.7854)  //Ŀ��㷽λ�ǳ���������Χ
		return 2;
	while (1) {
		z = position[2] - L1 + L4 * sin(pitch);  //��ؽ�z����
		r = L4 * cos(pitch);  //��ؽ���ĩ��ִ������xoyƽ���ͶӰ�ľ���
		x = position[0] - r * cos(theta[0]);  //��ؽ�x����
		y = position[1] - r * sin(theta[0]);  //��ؽ�y����
		r = sqrt(x * x + y * y + z * z);  //��ؽ���ԭ��ľ���
		costheta = (L2 * L2 + L3 * L3 - r * r) / L2 / L3 / 2.0;  //��L2��L3�ļн�
		if ((costheta < -1) || (costheta > 1)) goto PITCHADD;  //�н���Ч
		theta[2] = -acos(-costheta);  //ȡ������Ϊ�ؽڽǶ�,�ڶ��ʱȡ�ⲿ���Ͻ�
		if (ABS(theta[2] - ResetJoint[2]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
		targetPitch = atan2(z, sqrt(x * x + y * y));  //����ؽ������ԭ�������
		costheta = (L2 * L2 + r * r - L3 * L3) / L2 / r / 2.0;  //��L2����(��ؽ�)����ؽ�����֮��
		theta[1] = targetPitch + acos(costheta);  //��L2����
		if (ABS(theta[1] - ResetJoint[1]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
		theta[3] = PI / 2.0 - pitch - theta[1] - theta[2];  //L4�����L3������
		if (ABS(theta[3] - ResetJoint[3]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
		return 0;
	PITCHADD:  //����ĩ�˹ؽ��������¼���
		pitch += (pitch >= 0.78) ? 0.01 : -0.01;
		if (pitch > 1.5708)
			pitch = 0.77;
		else if (pitch < -1.5708) {  //�޿��н�
			theta[1] = ResetJoint[1];
			theta[2] = ResetJoint[2];
			theta[3] = ResetJoint[3];
			return 3;
		}
	}
}
