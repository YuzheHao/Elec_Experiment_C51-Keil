/*
	main.c
	数码管的动态扫描显示
*/


#include <reg51.h>
#include <absacc.h>


//定义显示缓冲区（由定时中断程序自动扫描）
unsigned char DispBuf[8];

code unsigned char Tab[] =
	{//定义0123456789AbCdEF的数码管字型数据，其他显示字符需自行计算，如‘-’的字形数据为0x40
		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
		0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
	};


/*
函数：T1INTSVC()
功能：定时器T1的中断服务函数
*/
void T1INTSVC() interrupt 3	//定时器1的中断号为：3
{
	code unsigned char com[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};	//显示位的端口控制字节
	static unsigned char n = 0;	//n: 扫描显示位计数,0-7
	TR1 = 0;
	TH1 = 0xFA;
	TL1 = 0x24;					//可以将FA24换成0000，降低扫描速度，观察和理解动态扫描
	TR1 = 1;
	XBYTE[0x7800] = 0xFF;		//暂停显示
	XBYTE[0x7801] = ~DispBuf[n];	//更新扫描显示数据
	XBYTE[0x7800] = ~com[n];	//重新显示
	n++;					 	//指向下一位扫描显示
	n &= 0x07;
}


/*
函数：DispClear()
功能：清除数码管的所有显示
*/
void DispClear()
{
	unsigned char i;
	for ( i=0; i<8; i++ )
	{
		DispBuf[i] = 0x00;	//i值代表数码管的位数，可以在后面的程序观察是左起还是右起，0x00可以关闭数码管显示
	}
}




/*
函数：DispInit()
功能：数码管扫描显示初始化
*/
void DispInit()
{
	DispClear();	//初始为全灭
	EA = 0;
	TH1 = 0xFA;		//1ms定时初值
	TL1 = 0x24;
	TR1 = 1;
	ET1 = 1;
	EA = 1;
}


/*
函数：Delay()
功能：延时0.01～2.56s
参数：
	t>0时，延时(t*0.01)s
	t=0时，延时2.56s
*/
void Delay(unsigned int t)
{
	do
	{
		TH0 = 0xC5;
		TL0 = 0x68;	 //10ms定时初值
		TR0 = 1;
		while ( !TF0 );
		TR0 = 0;
		TF0 = 0;
	} while ( --t != 0 );
}


/*
函数：SysInit()
功能：系统初始化
*/
void SysInit()
{

	TMOD = 0x11;	//设置定时器T0和T1为16位定时器
	DispInit();		//数码管扫描显示初始化
}


void main()
{
	unsigned char sec;		//秒计数
	unsigned char ms10;		//10ms计数
	unsigned char dig0;		//10ms数字位
	unsigned char dig1;		//100ms数字位
	unsigned char dig2;		//秒数字个位
	unsigned char dig3;		//秒数字十位
	unsigned char flag;
	flag=1;	

	SysInit();

	for(;;)
	{
		flag=!flag;
		for ( sec=0; sec<4; sec++ )		//显示00.00--99.99秒
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
						DispBuf[2] = Tab[dig2] | 0x80;	// 该位带小数点显示
						DispBuf[1] = Tab[dig1];
						DispBuf[0] = Tab[dig0];			
						break;
					case 1:
						DispBuf[7] = Tab[dig2] | 0x80;	// 该位带小数点显示
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
	
				Delay(1);						//毎0.01s(10ms)更新显示一次
			}
		}
	}

}

