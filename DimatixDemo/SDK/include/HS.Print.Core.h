#pragma once

#include "Common.h"

#ifdef HSPRINTCORE_EXPORTS
#define HSPRINTAPI __declspec(dllexport)
#else
#define HSPRINTAPI __declspec(dllimport)
#endif // DEBUG

#define HSFNTYPE __stdcall  //对于x64程序该标志自动被忽略（因x64已经将调用约定统一，只有x86才有区分）

//C接口
#ifdef __cplusplus
extern "C" {
#endif

	//获取错误码详情
	//小于0的错误码信息在此获取，大于0的的为系统错误，0是无错误
	HSPRINTAPI const Char * HSFNTYPE Ink_Print_GetErrInfo(int errCode);

	/*************************** 图形计算 ****************************/

	//从图形文件中生成图形
	//bmpFile：图形路径
	//cfg：切割配置
	//arrSwatheInfo: swatheinfo数组，只能通过DisposeSwatheInfo释放内存
	//count：arrSwatheInfo元素的个数
	//返回值：
	HSPRINTAPI int HSFNTYPE GenSwatheInfoFromFile(const Char * file, const SwatheSplitCfg * cfg, PrintInfo ** printInfo);

	//从内存图形中生成图形
	//data:输入的内存图像（该内存在此处不会被释放）
	HSPRINTAPI int HSFNTYPE GenSwatheInfoFromBuff(const ImageData * data, const SwatheSplitCfg * cfg, PrintInfo ** printInfo);

	//释放所有生成的SwatheInfo数组
	HSPRINTAPI void HSFNTYPE DeletePrintInfo(PrintInfo ** printInfo);

	/*************************** 喷印相关 ****************************/

	//连接
	HSPRINTAPI int HSFNTYPE Print_Connect(unsigned int IPAddress, unsigned short PortNum);

	//断开连接
	HSPRINTAPI void HSFNTYPE Print_Disconnect();

	//预初始化，主要开启脉冲电源（调用后10秒内不得调用下列Print_开头的接口）
	HSPRINTAPI int HSFNTYPE Print_PreInit();

	//初始化喷头类型并设置默认波形
	HSPRINTAPI int HSFNTYPE Print_Init(PHType type);

	/********* 配置 ********/

	//设置喷头类型
	HSPRINTAPI int HSFNTYPE Print_SetPHType(PHType type);

	//获取当前选择的喷头类型
	HSPRINTAPI PHType HSFNTYPE Print_GetSelPHType();

	//设置喷头的工作频率（覆盖现有波形设置的频率）
	//uFireFreq:(0-20kHz)
	HSPRINTAPI int HSFNTYPE Print_SetFireFreq(unsigned int freq);

	//设置波形
	HSPRINTAPI int HSFNTYPE Print_SetWaveform(const WaveformInfo * waveform);

	/********* 喷印 ********/

	//设置喷印配置（在DownloadSwathe前使用一次即可）
	HSPRINTAPI int HSFNTYPE Print_SetPrintCfg(PrintCfg * cfg);

	//下载图形
	//port:默认为0
	//swathe：打印的图像
	//checkErr:1:校验 0：不校验 （是否校验下发的swathe准确收到，会将下载的数据上传对比，比较耗时）
	HSPRINTAPI int HSFNTYPE Print_DownloadSwathe(int port, const SwatheInfo * swathe, int checkErr);
	//ignorePlsNum:从开始打印位置忽略多少光栅数目个脉冲后开始喷印（忽略的距离= 光栅尺分辨率 * ignorePlsNum）
	HSPRINTAPI int HSFNTYPE Print_DownloadSwatheEx(int port, const SwatheInfo * swathe, int checkErr, int ignorePlsNum);

	//开始打印（切换图形时需要停止打印再下载新图形）
	HSPRINTAPI int HSFNTYPE Print_EnterPrintMode();

	//停止打印
	HSPRINTAPI int HSFNTYPE Print_ExitPrintMode();


	/******* 墨滴观测 ********/

	//开启/关闭喷头测试
	//在墨滴观测/测量前都需要开启
	//isOn ：1-开启 0-关闭
	HSPRINTAPI int HSFNTYPE Print_SetTestNoz(int isOn);

	//闪频灯工作电流，用于调节灯亮度
	//current: 0-5000 （实际对应 0-500MA）
	HSPRINTAPI int HSFNTYPE Print_SetLampCurrent(unsigned short current);

	//调节相位
	//主要用于控制静态模式时墨滴的观测位置
	//(影响闪频灯开启的时机，静态模式会造成看到的墨点往下或往上移动)
	//lum:延时时间微秒us
	HSPRINTAPI int HSFNTYPE Print_SetLampPhase(unsigned int lum);

	//设置墨滴观测需要开启的孔
	//port:默认为0(0-3)
	//arrNozz:设置需要开启的孔(内存分布于单色位图的分布相同，开启的孔对应bit为1，否则为0，1字节对齐)
	//bytes：为arrNozz数组长度
	HSPRINTAPI int HSFNTYPE Print_SetNozOpen(int port, unsigned char *arrNozz, int bytes);

	//切换墨滴观测模式
	//isOn ：1- 慢镜头（飞行）模式，
	//	     0 -静态模式（用于测量）
	HSPRINTAPI int HSFNTYPE Print_SetFlyMode(int isOn);

	//设置慢镜头时，墨滴飞行速度
	//speed：1-10,可默认设置2
	HSPRINTAPI int HSFNTYPE Print_SetFlySpeed(unsigned char speed);

	/*********** 闪喷 ***********/

	//开启停止闪喷
	//uFireFreq:喷射频率
	//openTime：开启的时间长度ms
	//closeTime：关闭的时间长度ms
	HSPRINTAPI int HSFNTYPE Print_StartSpitting(unsigned int freq, int openTime, int closeTime);
	HSPRINTAPI int HSFNTYPE Print_StopSpitting();


	/*************************** 气墨路相关 ****************************/

	////打开设备
	////返回值：0-成功，-1-数据错误， 其他为错误码
	//HSPRINTAPI int HSFNTYPE AirInk_Open(const Char * portName);
	////关闭端口
	//HSPRINTAPI void HSFNTYPE AirInk_Close();
	////是否连接
	////返回值：0-断开 1-已连接
	//HSPRINTAPI int HSFNTYPE AirInk_IsConnected();


	////设置比例阀开度值
	////isOpen:false时range无效
	////open：开度值0-4095
	//HSPRINTAPI int HSFNTYPE AirInk_SetVacuum(int isOpen, unsigned short range);
	////meniscus:半月面压力(0-5kPa)，默认0.6kPa
	////extrudeInk:挤墨压力(0-50kPa)，默认35kPa
	//HSPRINTAPI int HSFNTYPE AirInk_SetPressure(float meniscus, float extrudeInk);

	////开启单次挤墨，超过1-2秒后自动关闭
	//HSPRINTAPI int HSFNTYPE AirInk_ExtrudeInkOnce();


	////查询控制板传感器实际测量值
	//HSPRINTAPI int HSFNTYPE AirInk_GetCfgParams(float *meniscus, float* extrudeInk, unsigned short *vacRange);

	////查询版本号
	//HSPRINTAPI int HSFNTYPE AirInk_GetVersion(int *major, int *minor);


#ifdef __cplusplus
}
#endif
