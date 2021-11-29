// UnitTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <cstring>
#include <Windows.h>
#include <HS.Print.Core.h>

#pragma comment(lib,"HS.Print.Core.lib")

#if defined(UNICODE) || defined(_UNICODE) 
#define Strcpy_s wcscpy_s
#else
#define Strcpy_s strcpy_s
#endif

#define SELPH   PHTSE128
#define CHNTYPE WFChnAddr1   //Ĭ��ͨ��1��ȡ�����û�����Ķ˿�

#define YDPI 50
#define XDPI 50
#define NOZNUM 128

#if defined(UNICODE) || defined(_UNICODE) 

#define Sprintf_s swprintf_s  //����ʵ��ռ�õ��ڴ��ֽ���,������������
#define Sscanf_s swscanf_s

//�������飬��_MaxCount����Ϊ_TRUNCATE���ɣ�����-1˵���ض�
//Snprintf_s���ܽ����������еĿɱ�������룬�����ȡ���ݲ���
#define Snprintf_s _snwprintf_s
#define Vsnprintf_s _vsnwprintf_s
//����΢���ʵ���������������ֱ�Ӷ���ʧ�ܣ������˳�������ϵͳ�᷵��-1
#define Vsprintf_s vswprintf_s
//��ȡ����
#define Strlen wcslen
#define Strcpy_s wcscpy_s
#define Strcat_s wcscat_s
#define	Strcmp wcscmp

#else

#define Sprintf_s sprintf_s
#define Sscanf_s sscanf_s

//�������飬��_MaxCount����Ϊ_TRUNCATE���ɣ�����-1˵���ض�
#define Snprintf_s _snprintf_s
#define Vsnprintf_s _vsnprintf_s
//����΢���ʵ���������������ֱ�Ӷ���ʧ�ܣ������˳�������ϵͳ�᷵��-1
#define Vsprintf_s vsprintf_s

#define Strlen strlen
#define Strcpy_s strcpy_s
#define Strcat_s strcat_s
#define	Strcmp strcmp
#endif //UNICODE


void GetFileName(Char file[MAX_PATH])
{
	::GetModuleFileName(nullptr, file, MAX_PATH);

	size_t startIndex = Strlen(file);

	while (--startIndex >= 0)
	{
		if (file[startIndex] == _T('\\') || file[startIndex] == _T('/'))
			if (startIndex + 1 < MAX_PATH)
			{
				file[startIndex + 1] = _T('\0');
				break;
			}
	}

	Strcat_s(file, MAX_PATH, _T("QS256.bmp"));
}


int Print()
{
	bool isOk = true;

	int freq = 2000;
	int ret;
	Char file[MAX_PATH];
	SwatheSplitCfg splitCfg;

	PrintInfo * printInfo = nullptr;
	//����Ĭ�ϲ����ļ�
	WaveformInfo waveform = { 1,  4, 1, 45, WFChnAddr1 };

	memset(&splitCfg, 0, sizeof(SwatheSplitCfg));


	splitCfg.PHType = SELPH;
	splitCfg.PrintDPI = XDPI;
	splitCfg.StartNoz = 0;
	splitCfg.EndNoz = NOZNUM - 1;

	splitCfg.IsSave = true;

	Strcpy_s(splitCfg.SaveDir, sizeof(splitCfg.SaveDir) / sizeof(splitCfg.SaveDir[0]), _T("D:\\Test"));
	GetFileName(file);
	ret = GenSwatheInfoFromFile(file, &splitCfg, &printInfo);

	if (ret != 0)
	{
		printf("GenSwatheInfoFromFileʧ��,������%d\r\n", ret);
		goto labErr;
	}


	while (true)
	{
		isOk = true;
		printf("�����ò��β���,(Rise,Due,Fall,Vol)/(1,4,1,45): \r\n");

		int x = scanf("%u,%u,%u,%u", &waveform.Rise, &waveform.Due, &waveform.Fall, &waveform.Vol);

		if (waveform.Rise > 5 || waveform.Rise < 1)
		{
			printf("��Ч��Rise(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Due > 19 || waveform.Due < 1)
		{
			printf("��Ч��Due(1-19) \r\n");
			isOk = false;
		}

		if (waveform.Fall > 5 || waveform.Fall < 1)
		{
			printf("��Ч��Fall(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Vol > 120 || waveform.Vol < 1)
		{
			printf("��Ч��Vol(1-120) \r\n");
			isOk = false;
		}

		if (isOk)
			break;
		else
			printf("������!\r\n\r\n");
	}


	printf("Enter print mode ...\r\n");


	//���ò��ε�ѹ
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform1ʧ��,������%d\r\n", ret);;
		goto labErr;
	}

	waveform.ChnAddr = WFChnAddr2;
	ret = Print_SetWaveform(&waveform);

	if (ret != 0)
	{
		printf("Print_SetWaveform2ʧ��,������%d\r\n", ret);;
		goto labErr;
	}


	//��������Ƶ��
	ret = Print_SetFireFreq(freq);

	//���ı���ͷʱʹ��
	//ret = Print_SetPHType(cfg.PHType);
	for (int step = 0; step < printInfo->StepNum; step++)
	{
		for (int pass = 0; pass < printInfo->PassNum; pass++)
		{
			int imgIndex = step * printInfo->PassNum + pass;
			if ((imgIndex + 1) > printInfo->SwatheCount)
				break;

			ret = Print_DownloadSwatheEx(0, printInfo->SwatheInfoSet[imgIndex], 1, 0);

			if (ret != 0)
			{
				printf("Print_DownloadSwatheExʧ��,������%d\r\n", ret);;
				goto labErr;
			}
			//������Ƿ񵽴��ӡ��ʼλ

			//֪ͨ���Ͽ�ʼ��ӡ�����ص�ͼ��
			ret = Print_EnterPrintMode();
			if (ret != 0)
			{
				printf("Print_EnterPrintModeʧ��,������%d\r\n", ret);;
				goto labErr;
			}
			//����˶�
			printf("���ƶ����Ŀ�ʼ��ӡ...\r\n");

			//�ȴ��������
			printf("�ȴ�����˶���ɲ��ɹ����أ���ɺ󰴻س�����...\r\n");
			system("pause");


			//֪ͨ�������ֹͣ��ͼ�Ĵ�ӡ
			Print_ExitPrintMode();

			//�������
		}

	}

	DeletePrintInfo(&printInfo);
	return 0;

labErr:
	DeletePrintInfo(&printInfo);
	return ret;
}

//noz:0-n
inline int GetNozPos(int noz, int *index)
{
	*index = (7 - (noz % 8));
	return  noz / 8;
}

void Observe()
{
	printf("Enter observe mode ...\r\n");

	int ret;
	unsigned char arrNozz[32] = { 0 };//���256�ף�32*8

	int nozNum = NOZNUM;
	int bytes = nozNum / 8 + ((nozNum % 8 != 0) ? 1 : 0);//������Ч�����ݳ���
	unsigned int lum = 45;
	unsigned short current = 500;
	unsigned char flySpeed = 3;
	int isFlyMode = 1;
	int freq = 2000;
	//����Ĭ�ϲ����ļ�
	WaveformInfo waveform = { 1,  4, 1, 50, CHNTYPE };
	//2 6 2 50
	//���ÿ���������
	{
		int bytes, index;
		memset(arrNozz, 0, 32);

		int noz;//(0-n)
		noz = 4;
		{
			bytes = GetNozPos(noz, &index);
			arrNozz[bytes] |= 0x01 << index;
		}

		//noz = 31;
		//{
		//	bytes = GetNozPos(noz, &index);
		//	arrNozz[bytes] |= 0x01 << index;
		//}
		//noz = 33;
		//{
		//	bytes = GetNozPos(noz, &index);
		//	arrNozz[bytes] |= 0x01 << index;
		//}
		//��������
		for (int noz = 0; noz < nozNum; noz++)
		{
			bytes = GetNozPos(noz, &index);
			arrNozz[bytes] |= 0x01 << index;
		}
	}

	//���ò��ε�ѹ
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform failed.");
		return;
	}

	//��������Ƶ��
	ret = Print_SetFireFreq(freq);


	//�رշ���ģʽ
	ret = Print_SetFlyMode(isFlyMode);
	//������λ
	ret = Print_SetLampPhase(lum);
	//���õƵ���
	ret = Print_SetLampCurrent(current);


	//���ÿ���������
	ret = Print_SetNozOpen(0, arrNozz, bytes);
	//��������
	ret = Print_SetTestNoz(true);
	//����ģʽ�µ��ٶ�
	ret = Print_SetFlySpeed(flySpeed);

	printf("Press enter to exit observe...\r\n");
	getchar();
	//�ر�
	ret = Print_SetTestNoz(false);
}


int _tmain(int argc, _TCHAR* argv[])
{
	int ret;

	unsigned int chn = 0;
	PrintCfg prtCfg;
	prtCfg.EncoderRes = 0.5;
	prtCfg.RequiredDPI = YDPI;

	printf("��������ֱ���(um): \r\n");
	scanf("%f", &prtCfg.EncoderRes);

	ret = Print_SetPrintCfg(&prtCfg);


	while (true)
	{
		printf("��������Ҫ��ӡ��ͨ����(ex��1/2/3/4): \r\n");

		scanf("%u", &chn);

		if ((chn < 1 || chn >4))
			printf("��Чͨ���ţ�������!\r\n\r\n");
		else
			break;
	}
	chn = DEF_IPADDRESS + chn - 1;


	ret = Print_Connect(chn, DEF_PORT);
	if (ret != 0)
	{
		printf("��ӡ������ʧ�� ...\r\n");
		goto LabErr;
	}

	printf("Print_PreInit ...\r\n");
	ret = Print_PreInit();
	printf("Print_PreInit %s \r\n", ret == 0 ? "ok" : "failed");
	if (ret != 0)
		goto LabEnd;

	//�ȴ�10s
	printf("Wait 15 seconds ...\r\n");
	Sleep(15000);
	printf("Print_Init ...\r\n");
	ret = Print_Init(SELPH);
	printf("Print_Init %s \r\n", ret == 0 ? "ok" : "failed");
	if (ret != 0)
		goto LabEnd;

	bool isOk;
	WaveformInfo waveform = { 1,  4, 1, 45, WFChnAddr1 };

	printf("Print_StartSpitting ...\r\n");
	while (true)
	{
		isOk = true;
		printf("�����ò��β���,(Rise,Due,Fall,Vol)/(1,4,1,45): \r\n");

		int x = scanf("%u,%u,%u,%u", &waveform.Rise, &waveform.Due, &waveform.Fall, &waveform.Vol);

		if (waveform.Rise > 5 || waveform.Rise < 1)
		{
			printf("��Ч��Rise(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Due > 19 || waveform.Due < 1)
		{
			printf("��Ч��Due(1-19) \r\n");
			isOk = false;
		}

		if (waveform.Fall > 5 || waveform.Fall < 1)
		{
			printf("��Ч��Fall(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Vol > 120 || waveform.Vol < 1)
		{
			printf("��Ч��Vol(1-120) \r\n");
			isOk = false;
		}

		if (isOk)
			break;
		else
			printf("������!\r\n\r\n");
	}
	//���ò��ε�ѹ
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform1ʧ��,������%d\r\n", ret);;
		goto LabErr;
	}

	waveform.ChnAddr = WFChnAddr2;
	ret = Print_SetWaveform(&waveform);

	if (ret != 0)
	{
		printf("Print_SetWaveform2ʧ��,������%d\r\n", ret);;
		goto LabErr;
	}

	ret = Print_StartSpitting(5000,1700,300);
	printf("Print_StartSpitting %s \r\n", ret == 0 ? "ok" : "failed");
	if (ret != 0)
		goto LabEnd;
	system("pause");

	Print_StopSpitting();

	int time = 1;
	//��ӡ����
	while (true)
	{
		printf("************************************* Print **********************************\r\n");
		printf("�����%d�δ�ӡ ...\r\n", time);
		ret = Print();
		printf("************************************* End ************************************\r\n");
		if (0 != ret)
			break;

		time++;
	}
	//ī�ι۲�
	//Observe();

LabEnd:

	//�Ͽ�����
	Print_Disconnect();

LabErr:
	system("pause");
	getchar();
	return ret;
}

