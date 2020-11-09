#include "inverse.h"
#include <cmath>
//#include <iostream>
//using namespace std;
#define LIMIT(x,min,max)         (((x)<=(min) ? (min) : ((x)>=(max) ? (max) : (x))))

constexpr double PI = 3.14159265358979323846;
//���ؽڳ���
constexpr double L1 = 93;
constexpr double L2 = 80;
constexpr double L3 = 81;
constexpr double L4 = 172;

/**********************
���˶�ѧ�����ؽڽǶ�
*@position:Ŀ�������
*@theta:���븴λʱ��DH����,��������ؽڲ���
*@return:����Ƿ����
**********************/
int Inverse_Solve(const double position[], double theta[])
{
	double x = position[0];
	double y = position[1];
	double z = position[2] - L1;
	double r = x * x + y * y + z * z;
	double ResetJoint[4];
	double pitch = 0.78;  //ĩ��ִ��������ؽ������Ŀ��������
	double targetPitch;  //ĩ��ִ��������ؽ�����ڹ̶�����ϵԭ�������
	double costheta1, costheta2;
	for (int i = 0; i < 4; i++)
		ResetJoint[i] = theta[i];
	if (r > (L2 + L3 + L4) * (L2 + L3 + L4))  //Ŀ��㳬����������
		return -1;
	theta[0] = atan2(y, x);
	if (abs(theta[0] - ResetJoint[0]) >= 0.7854)  //Ŀ��㷽λ�ǳ���������Χ
		return -2;
	while (1) {
		z = position[2] - L1 + L4 * sin(pitch);  //��ؽ�z����
		r = L4 * cos(pitch);  //��ؽ��ڹ̶�����ϵxoyƽ���ͶӰ��ԭ��ľ���
		x = position[0] - r * cos(theta[0]);  //��ؽ�x����
		y = position[1] - r * sin(theta[0]);  //��ؽ�y����
		r = sqrt(x * x + y * y + z * z);  //��ؽ���ԭ��ľ���
		costheta2 = (L2 * L2 + L3 * L3 - r * r) / L2 / L3 / 2.0;  //��L2��L3�ļн�
		if ((costheta2 >= -1) && (costheta2 <= 1)) {  //�н���Ч
			theta[2] = -acos(-costheta2);  //ȡ������Ϊ�ؽڽǶ�,�ڶ��ʱȡ�ⲿ���Ͻ�
			if (abs(theta[2] - ResetJoint[2]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
			targetPitch = atan2(z, sqrt(x * x + y * y));  //���ⲿ�ؽ�����ڹ̶�����ϵԭ�������
			costheta1 = (L2 * L2 + r * r - L3 * L3) / L2 / r / 2.0;  //��L2���Ǻ��ⲿ�ؽ�����֮��
			theta[1] = targetPitch + acos(costheta1);  //��L2����
			if (abs(theta[1] - ResetJoint[1]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
			theta[3] = PI / 2.0 - pitch - theta[1] - theta[2];  //L4�����L3������
			if (abs(theta[3] - ResetJoint[3]) >= 0.7854) goto PITCHADD;  //���������Χ,������ǰ���
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