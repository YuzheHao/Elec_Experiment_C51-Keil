/*
	main.c
	�Ӽ�������
*/


#include <reg51.h>
#include <absacc.h>
#include <ctype.h>


//���尴��
sbit KEY1 = P2^2;		//���尴��1��'+'��������Ӧ���İ���SW_2
sbit KEY2 = P2^0;		//���尴��2��'-'��������Ӧ���İ���SW_3

code unsigned char Tab[] =
	{//����0123456789AbCdEF���������������
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};

sbit PWM = P0^0;

//������ʾ���������ɶ�ʱ�жϳ����Զ�ɨ�裩
unsigned char DispBuf[8];

//����PWM�������Ҳ���ǵ���LED�����ȵȼ�
#define PWM_MAX	8
					   
//����PWM��������Ϊ0��PWM_MAX-1��
unsigned char PwmValue;

/*
������KeyScan()
���ܣ�����ɨ��
���أ�ɨ�赽�ļ�ֵ
*/
unsigned char KeyScan()
{
	unsigned char k = '\0';
	if ( KEY1 == 0 ) k = '+';
	if ( KEY2 == 0 ) k = '-';
	return k;
}

/*
������T1INTSVC()
���ܣ���ʱ��T1���жϷ�����
*/
void T1INTSVC() interrupt 3
{
	code unsigned char com[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	static unsigned char n = 0;
	static unsigned char t = 0;
	TR1 = 0;
	TH1 = 0xFA;
	TL1 = 0x24;
	TR1 = 1;
	XBYTE[0x7800] = 0xFF;		//��ͣ��ʾ
	XBYTE[0x7801] = ~DispBuf[n];	//����ɨ������
	XBYTE[0x7800] = ~com[n];//	������ʾ
	n++	;
	n &= 0x07;

	t++;
	if ( t >= PWM_MAX-1 ) t = 0;	//PWM��������Ϊ��PWM_MAX*1ms = 20ms
	if ( t < PwmValue )
	{
		PWM = 1;		//PWM���ߵ�ƽʱ��(LED��)��PwmValue*1ms
	}
	else
	{
		PWM = 0;		//PWM���͵�ƽʱ��(LED��)����PWM_MAX-PwmValue��*1ms
	}
}


/*
������DispClear()
���ܣ��������ܵ�������ʾ
*/
void DispClear()
{
	unsigned char i;
	for ( i=0; i<8; i++ )
	{
		DispBuf[i] = 0x00;
	}
}



/*
������DispInit()
���ܣ������ɨ����ʾ��ʼ��
*/
void DispInit()
{
	DispClear();	//��ʼΪȫ��
	EA = 0;
	TH1 = 0xFA;
	TL1 = 0x24;
	TR1 = 1;
	ET1 = 1;
	EA = 1;
}


/*
������Delay()
���ܣ���ʱ0.01��2.56s
������
	t>0ʱ����ʱ(t*0.01)s
	t=0ʱ����ʱ2.56s
˵����
	������18MHz
*/
void Delay(unsigned char t)
{
	do
	{
		TH0 = 0xC5;		//��ʱʱ��10ms
		TL0 = 0x68;
		TR0 = 1;
		while ( !TF0 );
		TR0 = 0;
		TF0 = 0;
	} while ( --t != 0 );
}


/*
������SysInit()
���ܣ�ϵͳ��ʼ��
*/
void SysInit()
{
	TMOD = 0x11;	//���ö�ʱ��T0��T1Ϊ16λ��ʱ��
	DispInit();		//�����ɨ����ʾ��ʼ��
}


void main()
{

	unsigned char k;		//�����ֵ����
	unsigned char cnt = 14;	//�������ֵ�����ĳ�ʼֵ

	SysInit();				//ϵͳ��ʼ��
	DispBuf[0] = Tab[cnt % 10];
	DispBuf[1] = Tab[cnt / 10];

	for (;;)
	{
		Delay(1);		//��ʱ10ms
		k = KeyScan();	//����ɨ��
		switch ( k )		//�жϼ�ֵ��ִ�о��幦��
		{
		case '+':
			if ( cnt < 18 ) cnt++;
			break;
		case '-':
			if ( cnt > 11 ) cnt--;
			break;
		default:
			break;
		}
		PwmValue = 18-cnt;
		DispBuf[0] = Tab[cnt % 10];
		DispBuf[1] = Tab[cnt / 10];

		for (;;)
		{
			Delay(1);		//��ʱ50ms
			if ( KeyScan() == '\0' ) break;	//��������ͷţ��˳�ѭ��
		}
	}
}

