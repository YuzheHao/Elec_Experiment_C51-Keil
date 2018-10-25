/*
	main.c
	����ܵĶ�̬ɨ����ʾ
*/


#include <reg51.h>
#include <absacc.h>


//������ʾ���������ɶ�ʱ�жϳ����Զ�ɨ�裩
unsigned char DispBuf[8];

code unsigned char Tab[] =
	{//����0123456789AbCdEF��������������ݣ�������ʾ�ַ������м��㣬�确-������������Ϊ0x40
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};


/*
������T1INTSVC()
���ܣ���ʱ��T1���жϷ�����
*/
void T1INTSVC() interrupt 3	//��ʱ��1���жϺ�Ϊ��3
{
	code unsigned char com[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};	//��ʾλ�Ķ˿ڿ����ֽ�
	static unsigned char n = 0;	//n: ɨ����ʾλ����,0-7
	TR1 = 0;
	TH1 = 0xFA;
	TL1 = 0x24;					//���Խ�FA24����0000������ɨ���ٶȣ��۲����⶯̬ɨ��
	TR1 = 1;
	XBYTE[0x7800] = 0xFF;		//��ͣ��ʾ
	XBYTE[0x7801] = ~DispBuf[n];	//����ɨ����ʾ����
	XBYTE[0x7800] = ~com[n];	//������ʾ
	n++;					 	//ָ����һλɨ����ʾ
	n &= 0x07;
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
		DispBuf[i] = 0x00;	//iֵ��������ܵ�λ���������ں���ĳ���۲�������������0x00���Թر��������ʾ
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
	TH1 = 0xFA;		//1ms��ʱ��ֵ
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
*/
void Delay(unsigned int t)
{
	do
	{
		TH0 = 0xC5;
		TL0 = 0x68;	 //10ms��ʱ��ֵ
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
	unsigned char sec;		//�����
	unsigned char ms10;		//10ms����
	unsigned char dig0;		//10ms����λ
	unsigned char dig1;		//100ms����λ
	unsigned char dig2;		//�����ָ�λ
	unsigned char dig3;		//������ʮλ
	unsigned char flag;
	flag=1;	

	SysInit();

	for(;;)
	{
		flag=!flag;
		for ( sec=0; sec<4; sec++ )		//��ʾ00.00--99.99��
		{
			for (ms10=0; ms10<100; ms10++)
			{
				dig1 = ms10/10;
				dig0 = ms10%10;
				dig3 = sec/10;
				dig2 = sec%10;
				switch(flag)
				{
					case 0:
						DispBuf[7] = Tab[9]; 
						DispBuf[6] = Tab[0];
						DispBuf[5] = Tab[5];
						DispBuf[4] = Tab[0];
						DispBuf[3] = 0x00;
						DispBuf[2] = Tab[dig2] | 0x80;	// ��λ��С������ʾ
						DispBuf[1] = Tab[dig1];
						DispBuf[0] = Tab[dig0];			
						break;
					case 1:
						DispBuf[7] = Tab[dig2] | 0x80;	// ��λ��С������ʾ
						DispBuf[6] = Tab[dig1];
						DispBuf[5] = Tab[dig0];
						DispBuf[4] = 0x00;
						DispBuf[3] = Tab[9]; 
						DispBuf[2] = Tab[0];
						DispBuf[1] = Tab[5];
						DispBuf[0] = Tab[0];
						break;
				}		
				if ( sec==3 )
				{
					break;
					sec=0;
					ms10=0;
				}
	
				Delay(1);						//��0.01s(10ms)������ʾһ��
			}
		}
	}

}

