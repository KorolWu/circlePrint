#pragma once

#pragma pack(push)
#pragma pack(4)

//C语言中没有bool类型
#if defined(UNICODE) || defined(_UNICODE) 
typedef wchar_t Char;
#else
typedef char Char;
#endif

//默认IP地址 192.128.3.114
#define DEF_IPADDRESS     0xc0a80372 
//默认连接端口号 1234
#define DEF_PORT	      1234


//喷头类型
enum PHType
{
	//SE128,128孔，50NPI
	PHTSE128 = 0,

	//QS256,256孔，100NPI
	PHTQS256 = 1,

	//卡夹,16孔，100NPI
	PHDMC116 = 2,

	//卡夹1PL,16孔，100NPI
	PHTDMC11601 = PHDMC116,

	//卡夹10PL,16孔，100NPI
	PHTDMC11610 = PHDMC116,

	//未知类型
	PHUnknown = 255,
};

//波形设置的通道地址
enum WFChnAddr
{
	WFChnAddr1 = 0,
	WFChnAddr2 = 1,
};


//图像信息
//X从左网右存储
typedef struct tagImageData
{
	//px
	int Width;
	int Height;
	//一个像素占用的比特数1,2,4,8bit
	int BitsPerPixel;
	//图像缓冲
	unsigned char *Data;

	//每行对齐的字节数(一般为1或4,默认设置1)
	unsigned char AlignBytes;
	//0 - Y是从上往下存储; 1 - Y是从下往上存储(默认设置0)
	unsigned char YDir;

	//索引图中那么小值为黑还是大值为黑（默认0）
	//0：小值为黑大值为白  1：大值为黑小值为白
	unsigned char BlackIndexVal;

}ImageData;


//喷印系统配置信息
typedef struct tagPrintCfg
{
	//打印DPI（Y轴运动方向）
	float RequiredDPI;
	//Y轴编码器分辨率um（喷印时只能使用外部编码器）
	float EncoderRes;

}PrintCfg;


//切割出的打印图像信息
//从上往下，从左往右，一个字节对接的数据
typedef struct tagSwatheInfo
{	
	//px
	int Width;
	int Height;
	//一个像素占用的比特数1,2,4,8bit（默认8）
	int BitsPerPixel;//HS版只支持单色位图
	//图像缓冲(1字节对齐的灰度图像，0-黑 255-白)
	unsigned char *Data;
	//数据长度（字节数）
	int Length;
	//喷头类型
	PHType PHType;

}SwatheInfo;

typedef struct tagPrintInfo
{	
	//一大步数量
	int StepNum;
	//一大步中小pass的数量
	int PassNum;

	//SwatheInfoSet数组中SwatheInfo *的数量(有效Swathe数量)
	int SwatheCount;
	//拆分的pass图像信息(SwatheInfo *数组)
	SwatheInfo ** SwatheInfoSet;

}PrintInfo;


//切割出的打印图像信息
typedef struct tagWaveformInfo
{
	//上升持续下降时间us(2-6-2-80)
	//上升沿：1-5us
	unsigned char Rise;
	//稳定时间：1-19us
	unsigned char Due;
	//下降沿：1-5us
	unsigned char Fall;
	//电压V：1-120V
	//SE128喷头不应超过90
	//卡夹不应超过50
	unsigned char Vol;
	//写入的通道
	WFChnAddr ChnAddr;

}WaveformInfo;


//生成Swathe的分割配置
typedef struct tagSwatheSplitCfg
{
	//喷头类型
	PHType PHType;
	//打印DPI(喷头分布/X轴移动方向)
	float PrintDPI;

	//起始孔和终止孔的索引号（0-n）
	unsigned int StartNoz;
	unsigned int EndNoz;

	//是否保存生成的图形（用于检测切割是否正确）
	//0-不保存，1-保存
	int IsSave;
	//保存图像的目录(目录后不带\)
	Char SaveDir[260];

}SwatheSplitCfg;

#pragma pack(pop)

