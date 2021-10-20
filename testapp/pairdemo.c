#include <stdio.h>
#include <signal.h>
#include <stdarg.h>

#include "PairInterface.h"

int g_exit = 0;
int g_dongle_attach_flag = 0;
int g_dongle_detach_flag = 0;

void OnLogCb(int level, const char *format, ...) 
{
    if (level > 5) 
	{
        return;
    }
    static char line[1024] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(line, sizeof(line), format, arg_ptr);
    va_end(arg_ptr);

    printf("EwPair::%s", line);

    return;

}

void OnUpgradeProgress(int cur,int total,int ok)
{
	static int percent = -1;
	if(!ok)
    {
    	if(percent != cur*100/total)
    	{
    		percent = cur*100/total;
    		printf("OnUpgradeProgress %%%d...\n",percent);
    	}
	}
	else
	{
		if(12345 == ok)
    		printf("OnUpgradeProgress file transfer complete...\n");
		else if(54321 == ok)
    		printf("OnUpgradeProgress upgrade ok ...\n");
	}
}

void OnDongleAttach() 
{
    printf("OnDongleAttach ...\n");
	g_dongle_attach_flag = 1;
	g_dongle_detach_flag = 0;
	
    return;
}

void OnDongleDetach() 
{
    printf("OnDongleDetach ..\n");
	g_dongle_detach_flag = 1;
	g_dongle_attach_flag = 0;
	
    return;
}

int startEwPairService() 
{
    printf("startEwPairService: %s \n", PAIRInterface_GetVersionInfo());

    PAIRPARAM PairCallbacks = {
            .notifyDongleAttach = OnDongleAttach,
            .notifyDongleDetach = OnDongleDetach,
            .writeLog  =  OnLogCb,
    };

    int ret = EwPairLibInit(&PairCallbacks);

    return ret;
}

void stopEwPairService() 
{
    EwPairLibDestroy();
    printf("stopEwPairService \n");
}

void sig_action(int sig) 
{
    stopEwPairService() ;
	
	g_exit = 1;
}

int main() 
{
	int paired = 10;
	int ver;
	
    signal(SIGINT, sig_action);
    signal(SIGTERM, sig_action);

    startEwPairService();

    while (g_exit == 0) 
	{
		if(g_dongle_attach_flag)
		{
			paired--;
			if(paired <= 0)
			{
				if(0 == PAIRInterface_DonglePair("192.168.43.132", "AndroidAP_1032", "11111111", "1"))
				{
					//paired = 10000000;
				}
				
				sleep(5);
				
				ver = PAIRInterface_GetDongleVer();
				if(ver > 0)
					printf("dongle ver %d\n",ver);
				//dir just for example
				//linux:/mnt/xxx/update.img
				//android:/data/local/tmp/update.img
#if 0
				if(0 == PAIRInterface_DongleUpgrade("/data/local/tmp/update.img",OnUpgradeProgress))
				{
					PAIRInterface_RebootDongle();
					//paired = 10000000;
				}
#endif
				
				sleep(2);
				
			}
		}
		if(g_dongle_detach_flag)
		{
			paired = 10;
		}
		
		sleep(1);
		
    }

    printf("Pair Demo exit!!!\n");
	
    return 0;

}
