#pragma once

#pragma pack(push)
#pragma pack(4)

//C������û��bool����
#if defined(UNICODE) || defined(_UNICODE) 
typedef wchar_t Char;
#else
typedef char Char;
#endif

//Ĭ��IP��ַ 192.128.3.114
#define DEF_IPADDRESS     0xc0a80372 
//Ĭ�����Ӷ˿ں� 1234
#define DEF_PORT	      1234


//��ͷ����
enum PHType
{
	//SE128,128�ף�50NPI
	PHTSE128 = 0,

	//QS256,256�ף�100NPI
	PHTQS256 = 1,

	//����,16�ף�100NPI
	PHDMC116 = 2,

	//����1PL,16�ף�100NPI
	PHTDMC11601 = PHDMC116,

	//����10PL,16�ף�100NPI
	PHTDMC11610 = PHDMC116,

	//δ֪����
	PHUnknown = 255,
};

//�������õ�ͨ����ַ
enum WFChnAddr
{
	WFChnAddr1 = 0,
	WFChnAddr2 = 1,
};


//ͼ����Ϣ
//X�������Ҵ洢
typedef struct tagImageData
{
	//px
	int Width;
	int Height;
	//һ������ռ�õı�����1,2,4,8bit
	int BitsPerPixel;
	//ͼ�񻺳�
	unsigned char *Data;

	//ÿ�ж�����ֽ���(һ��Ϊ1��4,Ĭ������1)
	unsigned char AlignBytes;
	//0 - Y�Ǵ������´洢; 1 - Y�Ǵ������ϴ洢(Ĭ������0)
	unsigned char YDir;

	//����ͼ����ôСֵΪ�ڻ��Ǵ�ֵΪ�ڣ�Ĭ��0��
	//0��СֵΪ�ڴ�ֵΪ��  1����ֵΪ��СֵΪ��
	unsigned char BlackIndexVal;

}ImageData;


//��ӡϵͳ������Ϣ
typedef struct tagPrintCfg
{
	//��ӡDPI��Y���˶�����
	float RequiredDPI;
	//Y��������ֱ���um����ӡʱֻ��ʹ���ⲿ��������
	float EncoderRes;

}PrintCfg;


//�и���Ĵ�ӡͼ����Ϣ
//�������£��������ң�һ���ֽڶԽӵ�����
typedef struct tagSwatheInfo
{	
	//px
	int Width;
	int Height;
	//һ������ռ�õı�����1,2,4,8bit��Ĭ��8��
	int BitsPerPixel;//HS��ֻ֧�ֵ�ɫλͼ
	//ͼ�񻺳�(1�ֽڶ���ĻҶ�ͼ��0-�� 255-��)
	unsigned char *Data;
	//���ݳ��ȣ��ֽ�����
	int Length;
	//��ͷ����
	PHType PHType;

}SwatheInfo;

typedef struct tagPrintInfo
{	
	//һ������
	int StepNum;
	//һ����Сpass������
	int PassNum;

	//SwatheInfoSet������SwatheInfo *������(��ЧSwathe����)
	int SwatheCount;
	//��ֵ�passͼ����Ϣ(SwatheInfo *����)
	SwatheInfo ** SwatheInfoSet;

}PrintInfo;


//�и���Ĵ�ӡͼ����Ϣ
typedef struct tagWaveformInfo
{
	//���������½�ʱ��us(2-6-2-80)
	//�����أ�1-5us
	unsigned char Rise;
	//�ȶ�ʱ�䣺1-19us
	unsigned char Due;
	//�½��أ�1-5us
	unsigned char Fall;
	//��ѹV��1-120V
	//SE128��ͷ��Ӧ����90
	//���в�Ӧ����50
	unsigned char Vol;
	//д���ͨ��
	WFChnAddr ChnAddr;

}WaveformInfo;


//����Swathe�ķָ�����
typedef struct tagSwatheSplitCfg
{
	//��ͷ����
	PHType PHType;
	//��ӡDPI(��ͷ�ֲ�/X���ƶ�����)
	float PrintDPI;

	//��ʼ�׺���ֹ�׵������ţ�0-n��
	unsigned int StartNoz;
	unsigned int EndNoz;

	//�Ƿ񱣴����ɵ�ͼ�Σ����ڼ���и��Ƿ���ȷ��
	//0-�����棬1-����
	int IsSave;
	//����ͼ���Ŀ¼(Ŀ¼�󲻴�\)
	Char SaveDir[260];

}SwatheSplitCfg;

#pragma pack(pop)

