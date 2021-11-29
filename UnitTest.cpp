// UnitTest.cpp : 定义控制台应用程序的入口点。
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
#define CHNTYPE WFChnAddr1   //默认通道1，取决于用户插入的端口

#define YDPI 50
#define XDPI 50
#define NOZNUM 128

#if defined(UNICODE) || defined(_UNICODE) 

#define Sprintf_s swprintf_s  //返回实际占用的内存字节数,不包含结束符
#define Sscanf_s swscanf_s

//有溢出检查，将_MaxCount设置为_TRUNCATE即可，返回-1说明截断
//Snprintf_s不能将包含函数中的可变参数传入，否则获取数据不对
#define Snprintf_s _snwprintf_s
#define Vsnprintf_s _vsnwprintf_s
//对于微软的实现如果缓冲区过大，直接断言失败，程序退出，其他系统会返回-1
#define Vsprintf_s vswprintf_s
//获取长度
#define Strlen wcslen
#define Strcpy_s wcscpy_s
#define Strcat_s wcscat_s
#define	Strcmp wcscmp

#else

#define Sprintf_s sprintf_s
#define Sscanf_s sscanf_s

//有溢出检查，将_MaxCount设置为_TRUNCATE即可，返回-1说明截断
#define Snprintf_s _snprintf_s
#define Vsnprintf_s _vsnprintf_s
//对于微软的实现如果缓冲区过大，直接断言失败，程序退出，其他系统会返回-1
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
	//设置默认波形文件
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
		printf("GenSwatheInfoFromFile失败,错误码%d\r\n", ret);
		goto labErr;
	}


	while (true)
	{
		isOk = true;
		printf("请设置波形参数,(Rise,Due,Fall,Vol)/(1,4,1,45): \r\n");

		int x = scanf("%u,%u,%u,%u", &waveform.Rise, &waveform.Due, &waveform.Fall, &waveform.Vol);

		if (waveform.Rise > 5 || waveform.Rise < 1)
		{
			printf("无效的Rise(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Due > 19 || waveform.Due < 1)
		{
			printf("无效的Due(1-19) \r\n");
			isOk = false;
		}

		if (waveform.Fall > 5 || waveform.Fall < 1)
		{
			printf("无效的Fall(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Vol > 120 || waveform.Vol < 1)
		{
			printf("无效的Vol(1-120) \r\n");
			isOk = false;
		}

		if (isOk)
			break;
		else
			printf("请重试!\r\n\r\n");
	}


	printf("Enter print mode ...\r\n");


	//设置波形电压
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform1失败,错误码%d\r\n", ret);;
		goto labErr;
	}

	waveform.ChnAddr = WFChnAddr2;
	ret = Print_SetWaveform(&waveform);

	if (ret != 0)
	{
		printf("Print_SetWaveform2失败,错误码%d\r\n", ret);;
		goto labErr;
	}


	//设置喷射频率
	ret = Print_SetFireFreq(freq);

	//当改变喷头时使用
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
				printf("Print_DownloadSwatheEx失败,错误码%d\r\n", ret);;
				goto labErr;
			}
			//检查电机是否到达打印起始位

			//通知马上开始打印刚下载的图形
			ret = Print_EnterPrintMode();
			if (ret != 0)
			{
				printf("Print_EnterPrintMode失败,错误码%d\r\n", ret);;
				goto labErr;
			}
			//电机运动
			printf("请移动基材开始打印...\r\n");

			//等待电机行走
			printf("等待电机运动完成并成功返回，完成后按回车继续...\r\n");
			system("pause");


			//通知行走完毕停止该图的打印
			Print_ExitPrintMode();

			//电机返回
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
	unsigned char arrNozz[32] = { 0 };//最多256孔：32*8

	int nozNum = NOZNUM;
	int bytes = nozNum / 8 + ((nozNum % 8 != 0) ? 1 : 0);//计算有效的数据长度
	unsigned int lum = 45;
	unsigned short current = 500;
	unsigned char flySpeed = 3;
	int isFlyMode = 1;
	int freq = 2000;
	//设置默认波形文件
	WaveformInfo waveform = { 1,  4, 1, 50, CHNTYPE };
	//2 6 2 50
	//设置开启的喷嘴
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
		//开启所有
		for (int noz = 0; noz < nozNum; noz++)
		{
			bytes = GetNozPos(noz, &index);
			arrNozz[bytes] |= 0x01 << index;
		}
	}

	//设置波形电压
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform failed.");
		return;
	}

	//设置喷射频率
	ret = Print_SetFireFreq(freq);


	//关闭飞行模式
	ret = Print_SetFlyMode(isFlyMode);
	//设置相位
	ret = Print_SetLampPhase(lum);
	//设置灯电流
	ret = Print_SetLampCurrent(current);


	//设置开启的喷嘴
	ret = Print_SetNozOpen(0, arrNozz, bytes);
	//开启测试
	ret = Print_SetTestNoz(true);
	//飞行模式下的速度
	ret = Print_SetFlySpeed(flySpeed);

	printf("Press enter to exit observe...\r\n");
	getchar();
	//关闭
	ret = Print_SetTestNoz(false);
}


int _tmain(int argc, _TCHAR* argv[])
{
	int ret;

	unsigned int chn = 0;
	PrintCfg prtCfg;
	prtCfg.EncoderRes = 0.5;
	prtCfg.RequiredDPI = YDPI;

	printf("请编码器分辨率(um): \r\n");
	scanf("%f", &prtCfg.EncoderRes);

	ret = Print_SetPrintCfg(&prtCfg);


	while (true)
	{
		printf("请输入需要喷印的通道号(ex：1/2/3/4): \r\n");

		scanf("%u", &chn);

		if ((chn < 1 || chn >4))
			printf("无效通道号，请重试!\r\n\r\n");
		else
			break;
	}
	chn = DEF_IPADDRESS + chn - 1;


	ret = Print_Connect(chn, DEF_PORT);
	if (ret != 0)
	{
		printf("喷印板连接失败 ...\r\n");
		goto LabErr;
	}

	printf("Print_PreInit ...\r\n");
	ret = Print_PreInit();
	printf("Print_PreInit %s \r\n", ret == 0 ? "ok" : "failed");
	if (ret != 0)
		goto LabEnd;

	//等待10s
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
		printf("请设置波形参数,(Rise,Due,Fall,Vol)/(1,4,1,45): \r\n");

		int x = scanf("%u,%u,%u,%u", &waveform.Rise, &waveform.Due, &waveform.Fall, &waveform.Vol);

		if (waveform.Rise > 5 || waveform.Rise < 1)
		{
			printf("无效的Rise(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Due > 19 || waveform.Due < 1)
		{
			printf("无效的Due(1-19) \r\n");
			isOk = false;
		}

		if (waveform.Fall > 5 || waveform.Fall < 1)
		{
			printf("无效的Fall(1-5) \r\n");
			isOk = false;
		}

		if (waveform.Vol > 120 || waveform.Vol < 1)
		{
			printf("无效的Vol(1-120) \r\n");
			isOk = false;
		}

		if (isOk)
			break;
		else
			printf("请重试!\r\n\r\n");
	}
	//设置波形电压
	ret = Print_SetWaveform(&waveform);
	if (ret != 0)
	{
		printf("Print_SetWaveform1失败,错误码%d\r\n", ret);;
		goto LabErr;
	}

	waveform.ChnAddr = WFChnAddr2;
	ret = Print_SetWaveform(&waveform);

	if (ret != 0)
	{
		printf("Print_SetWaveform2失败,错误码%d\r\n", ret);;
		goto LabErr;
	}

	ret = Print_StartSpitting(5000,1700,300);
	printf("Print_StartSpitting %s \r\n", ret == 0 ? "ok" : "failed");
	if (ret != 0)
		goto LabEnd;
	system("pause");

	Print_StopSpitting();

	int time = 1;
	//喷印流程
	while (true)
	{
		printf("************************************* Print **********************************\r\n");
		printf("进入第%d次打印 ...\r\n", time);
		ret = Print();
		printf("************************************* End ************************************\r\n");
		if (0 != ret)
			break;

		time++;
	}
	//墨滴观测
	//Observe();

LabEnd:

	//断开连接
	Print_Disconnect();

LabErr:
	system("pause");
	getchar();
	return ret;
}

