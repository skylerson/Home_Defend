#include "slc.h"


int monthTotal[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};



solar_lunar_s LunarToSolar(solar_lunar_s lunar) {
    int year = lunar.year,
        month = lunar.month,
        day = lunar.day;
    int byNow, xMonth, i;
    solar_lunar_s solar;
		
    byNow = (lunar200y[year-1901] & 0x001F) - 1;
    if( ((lunar200y[year-1901]&0x0060)>>5) == 2)
        byNow += 31;
    for(i = 1; i < month; i ++) 
		{
        if( ( lunar200y[year - 1901] & (0x80000 >> (i-1)) ) ==0) 
				{
            byNow += 29;
        }
        else
            byNow += 30;
    }
    byNow += day;
    xMonth = (lunar200y[year - 1901] & 0xf00000)>>20;
    if(xMonth != 0) 
			{
        if(month > xMonth  ||(month==xMonth && lunar.reserved == 1))
					{
            if((lunar200y[year-1901] & (0x80000>>(month-1)))==0)
                byNow += 29;
            else
                byNow += 30;
        }
    }
    if(byNow > 366 ||(year%4!=0 && byNow == 365)) 
			{
        year += 1;
        if(year%4==0)
            byNow -= 366;
        else
            byNow -= 365;
    }
    for(i=1; i <= 13; i ++) 
		{
        if(monthTotal[i] >= byNow) 
					{
            month = i;
            break;
        }
    }
    solar.day = byNow - monthTotal[month-1];
    solar.month = month;
    solar.year = year;

    return solar;
}








solar_lunar_s SolarToLunar(solar_lunar_s solar) 
{
    int year = solar.year,
        month = solar.month,
        day = solar.day;
    int bySpring,bySolar,daysPerMonth;
    int index,flag;
    solar_lunar_s lunar;

    //bySpring 记录春节离当年元旦的天数。
    //bySolar 记录阳历日离当年元旦的天数。
    if( ((lunar200y[year-1901] & 0x0060) >> 5) == 1)
        bySpring = (lunar200y[year-1901] & 0x001F) - 1;
    else
        bySpring = (lunar200y[year-1901] & 0x001F) - 1 + 31;
    bySolar = monthTotal[month-1] + day - 1;
    if( (!(year % 4)) && (month > 2))
        bySolar++;

    //daysPerMonth记录大小月的天数 29 或30
    //index 记录从哪个月开始来计算。
    //flag 是用来对闰月的特殊处理。

    //判断阳历日在春节前还是春节后
    if (bySolar >= bySpring) {//阳历日在春节后（含春节那天）
        bySolar -= bySpring;
        month = 1;
        index = 1;
        flag = 0;
        if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while(bySolar >= daysPerMonth) {
            bySolar -= daysPerMonth;
            index++;
            if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20) ) {
                flag = ~flag;
                if(flag == 0)
                    month++;
            }
            else
                month++;
            if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
                daysPerMonth=29;
            else
                daysPerMonth=30;
        }
        day = bySolar + 1;
    }
    else {//阳历日在春节前
        bySpring -= bySolar;
        year--;
        month = 12;
        if ( ((lunar200y[year - 1901] & 0xF00000) >> 20) == 0)
            index = 12;
        else
            index = 13;
        flag = 0;
        if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while(bySpring > daysPerMonth) {
            bySpring -= daysPerMonth;
            index--;
            if(flag == 0)
                month--;
            if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20))
                flag = ~flag;
            if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
                daysPerMonth = 29;
            else
                daysPerMonth = 30;
        }

        day = daysPerMonth - bySpring + 1;
    }
    lunar.day = day;
    lunar.month = month;
    lunar.year = year;
    if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20))
        lunar.reserved = 1;
    else
        lunar.reserved = 0;
    return lunar;
}



