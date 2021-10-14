#ifndef __PAIR_INTERFACE_H__
#define __PAIR_INTERFACE_H__

#if defined (WIN32) && defined(DLL_EXPORT)
# define EWPAIR_API __declspec(dllexport)
#else
# define EWPAIR_API
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//按键插入回调
typedef void(*PairNotifyDongleAttach)();

//按键拔出回调
typedef void(*PairNotifyDongleDetach)();

/*
 * 日志回调函数
 * int level: LOGV 5, LOGD 4, LOGI 3, LOGW 2, LOGE 1
 * */
typedef void (*PairLog)(int level, const char *format, ...);

typedef struct {
    PairNotifyDongleAttach notifyDongleAttach;
    PairNotifyDongleDetach notifyDongleDetach;
    PairLog writeLog;
} PAIRPARAM, *PPAIRPARAM;

//初始化并启动服务
EWPAIR_API int EwPairLibInit(PPAIRPARAM pPairParam);

EWPAIR_API int EwPairLibDestroy();

//获取libewpair版本信息
EWPAIR_API char *PAIRInterface_GetVersionInfo();

//获取按键程序版本号
//格式说明：比如V3.1.4，将返回整数 3<<16|1<<8|4
//错误：-1
EWPAIR_API int PAIRInterface_GetDongleVer();

//升级进度回调
//进度分2个过程：
//1、升级文件传输，cur/total为传输百分比，当cur==total时代表文件传输完毕，界面进度条需预留过程2进度
//2、升级文件升级，success==12345表示该过程开始，success==54321表示升级成功
typedef void(*NotifyDongleUpgradeProgress)(int cur,int total,int success);

//按键升级接口,file:升级文件，请确保文件有可读权限，该文件由宝疆提供
//返回值：
//-1-失败，0-成功
EWPAIR_API int PAIRInterface_DongleUpgrade(const char *file,NotifyDongleUpgradeProgress cb);

//按键配对接口,channel:5G的AP填写149，2.4G的AP填写1
//返回值：
//-1-失败，0-成功
EWPAIR_API int PAIRInterface_DonglePair(char *host_ip, char *ssid, char *pkey, char *channel) ;

#if defined(__cplusplus)
}
#endif

#endif 

