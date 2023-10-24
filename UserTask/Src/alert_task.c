#include "alert_task.h"


alert_info_s password_alert_info;

void alert_task(void)
{
	if( password_alert_info.timer >0)
	{
		
		password_alert_info.timer --;
	}
	else
	{
		
	}

}





