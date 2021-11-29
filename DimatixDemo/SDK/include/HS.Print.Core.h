#pragma once

#include "Common.h"

#ifdef HSPRINTCORE_EXPORTS
#define HSPRINTAPI __declspec(dllexport)
#else
#define HSPRINTAPI __declspec(dllimport)
#endif // DEBUG

#define HSFNTYPE __stdcall  //����x64����ñ�־�Զ������ԣ���x64�Ѿ�������Լ��ͳһ��ֻ��x86�������֣�

//C�ӿ�
#ifdef __cplusplus
extern "C" {
#endif

	//��ȡ����������
	//С��0�Ĵ�������Ϣ�ڴ˻�ȡ������0�ĵ�Ϊϵͳ����0���޴���
	HSPRINTAPI const Char * HSFNTYPE Ink_Print_GetErrInfo(int errCode);

	/*************************** ͼ�μ��� ****************************/

	//��ͼ���ļ�������ͼ��
	//bmpFile��ͼ��·��
	//cfg���и�����
	//arrSwatheInfo: swatheinfo���飬ֻ��ͨ��DisposeSwatheInfo�ͷ��ڴ�
	//count��arrSwatheInfoԪ�صĸ���
	//����ֵ��
	HSPRINTAPI int HSFNTYPE GenSwatheInfoFromFile(const Char * file, const SwatheSplitCfg * cfg, PrintInfo ** printInfo);

	//���ڴ�ͼ��������ͼ��
	//data:������ڴ�ͼ�񣨸��ڴ��ڴ˴����ᱻ�ͷţ�
	HSPRINTAPI int HSFNTYPE GenSwatheInfoFromBuff(const ImageData * data, const SwatheSplitCfg * cfg, PrintInfo ** printInfo);

	//�ͷ��������ɵ�SwatheInfo����
	HSPRINTAPI void HSFNTYPE DeletePrintInfo(PrintInfo ** printInfo);

	/*************************** ��ӡ��� ****************************/

	//����
	HSPRINTAPI int HSFNTYPE Print_Connect(unsigned int IPAddress, unsigned short PortNum);

	//�Ͽ�����
	HSPRINTAPI void HSFNTYPE Print_Disconnect();

	//Ԥ��ʼ������Ҫ���������Դ�����ú�10���ڲ��õ�������Print_��ͷ�Ľӿڣ�
	HSPRINTAPI int HSFNTYPE Print_PreInit();

	//��ʼ����ͷ���Ͳ�����Ĭ�ϲ���
	HSPRINTAPI int HSFNTYPE Print_Init(PHType type);

	/********* ���� ********/

	//������ͷ����
	HSPRINTAPI int HSFNTYPE Print_SetPHType(PHType type);

	//��ȡ��ǰѡ�����ͷ����
	HSPRINTAPI PHType HSFNTYPE Print_GetSelPHType();

	//������ͷ�Ĺ���Ƶ�ʣ��������в������õ�Ƶ�ʣ�
	//uFireFreq:(0-20kHz)
	HSPRINTAPI int HSFNTYPE Print_SetFireFreq(unsigned int freq);

	//���ò���
	HSPRINTAPI int HSFNTYPE Print_SetWaveform(const WaveformInfo * waveform);

	/********* ��ӡ ********/

	//������ӡ���ã���DownloadSwatheǰʹ��һ�μ��ɣ�
	HSPRINTAPI int HSFNTYPE Print_SetPrintCfg(PrintCfg * cfg);

	//����ͼ��
	//port:Ĭ��Ϊ0
	//swathe����ӡ��ͼ��
	//checkErr:1:У�� 0����У�� ���Ƿ�У���·���swathe׼ȷ�յ����Ὣ���ص������ϴ��Աȣ��ȽϺ�ʱ��
	HSPRINTAPI int HSFNTYPE Print_DownloadSwathe(int port, const SwatheInfo * swathe, int checkErr);
	//ignorePlsNum:�ӿ�ʼ��ӡλ�ú��Զ��ٹ�դ��Ŀ�������ʼ��ӡ�����Եľ���= ��դ�߷ֱ��� * ignorePlsNum��
	HSPRINTAPI int HSFNTYPE Print_DownloadSwatheEx(int port, const SwatheInfo * swathe, int checkErr, int ignorePlsNum);

	//��ʼ��ӡ���л�ͼ��ʱ��Ҫֹͣ��ӡ��������ͼ�Σ�
	HSPRINTAPI int HSFNTYPE Print_EnterPrintMode();

	//ֹͣ��ӡ
	HSPRINTAPI int HSFNTYPE Print_ExitPrintMode();


	/******* ī�ι۲� ********/

	//����/�ر���ͷ����
	//��ī�ι۲�/����ǰ����Ҫ����
	//isOn ��1-���� 0-�ر�
	HSPRINTAPI int HSFNTYPE Print_SetTestNoz(int isOn);

	//��Ƶ�ƹ������������ڵ��ڵ�����
	//current: 0-5000 ��ʵ�ʶ�Ӧ 0-500MA��
	HSPRINTAPI int HSFNTYPE Print_SetLampCurrent(unsigned short current);

	//������λ
	//��Ҫ���ڿ��ƾ�̬ģʽʱī�εĹ۲�λ��
	//(Ӱ����Ƶ�ƿ�����ʱ������̬ģʽ����ɿ�����ī�����»������ƶ�)
	//lum:��ʱʱ��΢��us
	HSPRINTAPI int HSFNTYPE Print_SetLampPhase(unsigned int lum);

	//����ī�ι۲���Ҫ�����Ŀ�
	//port:Ĭ��Ϊ0(0-3)
	//arrNozz:������Ҫ�����Ŀ�(�ڴ�ֲ��ڵ�ɫλͼ�ķֲ���ͬ�������Ŀ׶�ӦbitΪ1������Ϊ0��1�ֽڶ���)
	//bytes��ΪarrNozz���鳤��
	HSPRINTAPI int HSFNTYPE Print_SetNozOpen(int port, unsigned char *arrNozz, int bytes);

	//�л�ī�ι۲�ģʽ
	//isOn ��1- ����ͷ�����У�ģʽ��
	//	     0 -��̬ģʽ�����ڲ�����
	HSPRINTAPI int HSFNTYPE Print_SetFlyMode(int isOn);

	//��������ͷʱ��ī�η����ٶ�
	//speed��1-10,��Ĭ������2
	HSPRINTAPI int HSFNTYPE Print_SetFlySpeed(unsigned char speed);

	/*********** ���� ***********/

	//����ֹͣ����
	//uFireFreq:����Ƶ��
	//openTime��������ʱ�䳤��ms
	//closeTime���رյ�ʱ�䳤��ms
	HSPRINTAPI int HSFNTYPE Print_StartSpitting(unsigned int freq, int openTime, int closeTime);
	HSPRINTAPI int HSFNTYPE Print_StopSpitting();


	/*************************** ��ī·��� ****************************/

	////���豸
	////����ֵ��0-�ɹ���-1-���ݴ��� ����Ϊ������
	//HSPRINTAPI int HSFNTYPE AirInk_Open(const Char * portName);
	////�رն˿�
	//HSPRINTAPI void HSFNTYPE AirInk_Close();
	////�Ƿ�����
	////����ֵ��0-�Ͽ� 1-������
	//HSPRINTAPI int HSFNTYPE AirInk_IsConnected();


	////���ñ���������ֵ
	////isOpen:falseʱrange��Ч
	////open������ֵ0-4095
	//HSPRINTAPI int HSFNTYPE AirInk_SetVacuum(int isOpen, unsigned short range);
	////meniscus:������ѹ��(0-5kPa)��Ĭ��0.6kPa
	////extrudeInk:��īѹ��(0-50kPa)��Ĭ��35kPa
	//HSPRINTAPI int HSFNTYPE AirInk_SetPressure(float meniscus, float extrudeInk);

	////�������μ�ī������1-2����Զ��ر�
	//HSPRINTAPI int HSFNTYPE AirInk_ExtrudeInkOnce();


	////��ѯ���ư崫����ʵ�ʲ���ֵ
	//HSPRINTAPI int HSFNTYPE AirInk_GetCfgParams(float *meniscus, float* extrudeInk, unsigned short *vacRange);

	////��ѯ�汾��
	//HSPRINTAPI int HSFNTYPE AirInk_GetVersion(int *major, int *minor);


#ifdef __cplusplus
}
#endif
