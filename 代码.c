#include<reg51.h>
#include<absacc.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int

uchar xdata *keyin = 0x8001;
uchar xdata *digit = 0x8002;
uchar xdata *seg = 0x8004;

uchar code digit_c[] = { 0x01,0x02,0x04,0x08,0x10,0x20 };
uchar code seg_c[] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f };       //    ???LED????? 0 1 2 3 4 5 6 7 8 9 0 0 0 0 0 0 0;

uchar ppp;      
uchar key_status = 0;     
uchar keynum = 24;   


uchar minute = 11;
uchar second = 60;
uchar scoreA = 0;
uchar scoreB = 0;
uchar count1 = 20;
uchar rule_time = 24;
uchar rule_flag = 0;

sbit shi_or_fen = P3 ^ 2;

void time_init() {
	TMOD = 0x01;
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	EA = 1;
	ET0 = 1;
}

void delay_ms(uint z) {
	uint i;
	uchar j;
	for (i = 0; i < z; i++) {

		for (j = 0; j < 100; j++);
	}
}

void T0_Time() interrupt 1 using 1{
	if (count1-- == 0)
	{
		second--;
		rule_time--;
		count1 = 10;
	}
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
}
void jiance()
{
	*digit = ppp;
	ppp = *keyin;
	ppp = ~ppp;
	ppp &= 0x0f;
}
void scanline()
{
	ppp = 0xf7;
	jiance();
	if (ppp != 0) 
	{
		keynum = keynum + 3;
	}
	ppp = 0xef;
	jiance();
	if (ppp != 0) 
	{
		keynum = keynum + 4;
	}
	ppp = 0xdf;
	jiance();
	if (ppp != 0) 
	{
		keynum = keynum + 5;
	}
}
int scankey() {
	key_status = 0;
	ppp = 0;
	jiance();
	if (ppp != 0) 
	{
		delay_ms(20);
		ppp = 0;
		jiance();
		if (ppp != 0)
		{
			switch (ppp)
			{
			case 0x01:
				keynum = 0;
				scanline();
				break;
			case 0x02:
				keynum = 6;
				scanline();
				break;
			case 0x04:
				keynum = 12;
				scanline();
				break;
			case 0x08:
				keynum = 18;
				scanline();
				break;
			}
			while (1) {
				ppp = 0;
				jiance();
				if (ppp == 0)
				{
					key_status = 1;
					return  0;
				}
			}
		}

	}
	key_status = 0;
	return 1;
}

void time_display(uchar w2, uchar d2) {
	uchar shi1, ge1;
	shi1 = d2 / 10;
	ge1 = d2 % 10;

	*digit = digit_c[5-w2];
	*seg = seg_c[shi1];
	delay_ms(3);

	*digit = digit_c[4-w2];
	*seg = seg_c[ge1];
	delay_ms(3);

}

void score_dispaly(uchar w2, uchar d2) {
	uchar bai2, shi2, ge2;
	bai2 = d2 / 100;
	shi2 = d2 % 100 / 10;
	ge2 = d2 % 100 % 10;

	*digit = digit_c[5-w2];
	*seg = seg_c[bai2];
	delay_ms(3);

	*digit = digit_c[4-w2];
	*seg = seg_c[shi2];
	delay_ms(3);

	*digit = digit_c[3-w2];
	*seg = seg_c[ge2];
	delay_ms(3);

}

void main()
{
	time_init();
	
	while (1) {
		scankey();
		switch (keynum)
		{
		case 3:TR0 = 1;
			rule_flag = 1;
			break;  
		case 4:TR0 = 0;
			scoreB += 3;
			rule_time = 24;
			rule_flag = 0;
			break;
		case 5:TR0 = 0;
			scoreA += 3;
			rule_time = 24;
			rule_flag = 0;
			break;
		case 9: TR0 = 0;
			break;  
		case 10: TR0 = 0;
			scoreB += 2;
			rule_time = 24;
			rule_flag = 0;
			break;
		case 11:TR0 = 0;
			scoreA += 2;
			rule_time = 24;
			rule_flag = 0;
			break;
		case 15:if (TR0 == 1) {
			rule_time = 0;
			rule_flag = 0;
		}
			   else {
			rule_flag = 0;
		}
			   break;
		case 16: TR0 = 0;
			scoreB += 1;
			rule_time = 24;
			rule_flag = 0;
			break;
		case 17:TR0 = 0;
			scoreA += 1;
			rule_time = 24;
			rule_flag = 0;
			break;

		}
		if(second==0)
		{
		    if(minute==0)
			{
			  minute=11;
				second=60;
				rule_time=24;
				rule_flag=0;
				TR0=0;
			}
			else 
			{
			   minute--;
				 second=60;
			}
		}
		
		if(rule_time<=0)
		{
		  TR0=0;
			rule_time=24;
		}
		keynum=0;
		if (shi_or_fen == 1)
		{
			score_dispaly(0, scoreA);
			score_dispaly(3, scoreB);
		}
		else
		{
			time_display(0, minute);
			time_display(2, second);
			time_display(4, rule_time);
		}

	}
}
