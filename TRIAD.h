/*
 * TRIAD.h
 *
 *  Created on: Sep 6, 2017
 *      Author: hunteradams
 */

#ifndef TRIAD_H_
#define TRIAD_H_

//#include "LSM9DS1.h"

float crossProductX(float u1, float u2, float u3,
		            float v1, float v2, float v3)
{
	return u2*v3-u3*v2;
}

float crossProductY(float u1, float u2, float u3,
					float v1, float v2, float v3)
{
	return u3*v1-u1*v3;
}

float crossProductZ(float u1, float u2, float u3,
					float v1, float v2, float v3)
{
	return u1*v2-u2*v1;
}

float vectorMagnitude(float u1, float u2, float u3)
{
	return sqrt(u1*u1 + u2*u2 + u3*u3);
}

float* computeAttitude(int16_t mx, int16_t my, int16_t mz, int16_t ax, int16_t ay, int16_t az, float buffer[9])
{
	float mxf = calcMag(mx);
	float myf = calcMag(my);
	float mzf = calcMag(mz);
	float axf = calcAccel(ax);
	float ayf = calcAccel(ay);
	float azf = calcAccel(az);

	float asq = vectorMagnitude(axf, ayf, azf);

	float axn = axf/asq;
	float ayn = ayf/asq;
	float azn = azf/asq;


	float axm_x = crossProductX(axf, ayf, azf,
								mxf, myf, mzf);
	float axm_y = crossProductY(axf, ayf, azf,
									mxf, myf, mzf);
	float axm_z = crossProductZ(axf, ayf, azf,
									mxf, myf, mzf);

	float axm_sq = vectorMagnitude(axm_x, axm_y, axm_z);
	float axm_xn = axm_x/axm_sq;
	float axm_yn = axm_y/axm_sq;
	float axm_zn = axm_z/axm_sq;


	float anxaxm_n_x = crossProductX(axn, ayn, azn,
									axm_xn, axm_yn, axm_zn);
	float anxaxm_n_y = crossProductY(axn, ayn, azn,
									axm_xn, axm_yn, axm_zn);
	float anxaxm_n_z = crossProductZ(axn, ayn, azn,
									axm_xn, axm_yn, axm_zn);

	float anxaxm_sq = vectorMagnitude(anxaxm_n_x, anxaxm_n_y, anxaxm_n_z);

	float anxaxm_nxn = anxaxm_n_x/anxaxm_sq;
	float anxaxm_nyn = anxaxm_n_y/anxaxm_sq;
	float anxaxm_nzn = anxaxm_n_z/anxaxm_sq;

	float AXM_X = crossProductX(AXN, AYN, AZN,
								MXN, MYN, MZN);
	float AXM_Y = crossProductY(AXN, AYN, AZN,
								MXN, MYN, MZN);
	float AXM_Z = crossProductZ(AXN, AYN, AZN,
								MXN, MYN, MZN);

	float AXM_SQ = vectorMagnitude(AXM_X, AXM_Y, AXM_Z);
	float AXM_XN = AXM_X/AXM_SQ;
	float AXM_YN = AXM_Y/AXM_SQ;
	float AXM_ZN = AXM_Z/AXM_SQ;

	float ANXAXM_N_X = crossProductX(AXN, AYN, AZN,
									AXM_XN, AXM_YN, AXM_ZN);
	float ANXAXM_N_Y = crossProductY(AXN, AYN, AZN,
										AXM_XN, AXM_YN, AXM_ZN);
	float ANXAXM_N_Z = crossProductZ(AXN, AYN, AZN,
										AXM_XN, AXM_YN, AXM_ZN);

	float ANXAXM_SQ = vectorMagnitude(ANXAXM_N_X, ANXAXM_N_Y, ANXAXM_N_Z);

	float ANXAXM_NXN = ANXAXM_N_X/ANXAXM_SQ;
	float ANXAXM_NYN = ANXAXM_N_Y/ANXAXM_SQ;
	float ANXAXM_NZN = ANXAXM_N_Z/ANXAXM_SQ;


	float a11 = axn*AXN + anxaxm_nxn*ANXAXM_NXN + axm_xn*AXM_XN;
	float a12 = axn*AYN + anxaxm_nxn*ANXAXM_NYN + axm_xn*AXM_YN;
	float a13 = axn*AZN + anxaxm_nxn*ANXAXM_NZN + axm_xn*AXM_ZN;

	float a21 = AXN*ayn + anxaxm_nyn*ANXAXM_NXN + axm_yn*AXM_XN;
	float a22 = ayn*AYN + anxaxm_nyn*ANXAXM_NYN + axm_yn*AXM_YN;
	float a23 = ayn*AZN + anxaxm_nyn*ANXAXM_NZN + axm_yn*AXM_ZN;

	float a31 = AXN*azn + anxaxm_nzn*ANXAXM_NXN + axm_zn*AXM_XN;
	float a32 = AYN*azn + anxaxm_nzn*ANXAXM_NYN + axm_zn*AXM_YN;
	float a33 = azn*AZN + anxaxm_nzn*ANXAXM_NZN + axm_zn*AXM_ZN;

	buffer[0]=a11;
	buffer[1]=a12;
	buffer[2]=a13;
	buffer[3]=a21;
	buffer[4]=a22;
	buffer[5]=a23;
	buffer[6]=a31;
	buffer[7]=a32;
	buffer[8]=a33;
	return buffer;

//	Display_printf(display, 0, 0, "%f, %f, %f, %f, %f, %f, %f, %f, %f", a11, a12, a13, a21, a22, a23, a31, a32, a33);
}



#endif /* TRIAD_H_ */
