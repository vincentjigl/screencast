/* coding:utf-8 */
#ifndef _H_MAIN_INTERFACE_H
#define _H_MAIN_INTERFACE_H

#if defined (WIN32) && defined(DLL_EXPORT)
# define EWRECV_API __declspec(dllexport)
#else
# define EWRECV_API
#endif


#ifdef __cplusplus
extern "C" {
#endif

//投屏状态回调
typedef void(*MainNotifyMirroringStat)(int conn_id, int video_id, int status);

//连接状态回调
typedef void(*MainNotifyConnStat)(int connect_id, int stat, int conn_num, char *pIpAddr);

//设备名称和类型通知
//type: 0:dongle; 1:android; 4:windows sender app;
typedef void(*MainNotifyConnHostInfo)(int connect_id, int type, char *pDevName);

//reserve
typedef void(*MainNotifyResetEncoderResult)(int connect_id, int result);

//dongle按钮信息通知
//目前信息包含pc端版本号+dongle版本号
//具体解析查看demo程序
typedef void(*MainNotifyDongleInfo)(int connect_id,unsigned char *pinfo);

/*
 * 日志回调函数
 * int level: LOGV 5, LOGD 4, LOGI 3, LOGW 2, LOGE 1
 * */
typedef void (*MainLog)(int level, const char *format, ...);


typedef struct {

    int nMediaCh;  //媒体通道数，1~4;
    int nConnCh;   //最大允许的连接通道数，最大为16
    int nTcpPort;  //接收端服务端口：10000
    MainNotifyMirroringStat notifyMirroringStat;
    MainNotifyConnStat notifyConnStat;
    MainNotifyConnHostInfo notifyConnHostInfo;
    MainNotifyResetEncoderResult notifyResetEncoderResult;
    MainNotifyDongleInfo notifyDongleInfo;
    MainLog writeLog;
} MAINParam_st, *PMAINParam_st;


//创建播放器通知
typedef int(*DecoderRendererSetup)(int video_id, int conn_id);

//清理播放器通知
typedef void(*DecoderRendererCleanup)(int video_id);

//视频数据回调
typedef int(*DecoderRendererSubmitDecodeUnit)(int video_id, unsigned char *decodeUnit, int len);

//切换视频分辨率通知
typedef void (*DecoderRendererNotifyResolution)(int video_id, int width, int height);

//切换视频旋转角度通知
//rotate：0，90，180，270；
typedef void (*DecoderRendererNotifyRotate)(int video_id, int rotate);

//投屏模式为DirectX时切换视频旋转角度通知
//rotate：0，90，180，270；
typedef void (*DecoderRendererNotifyDirectxRotate)(int video_id, int rotate);

#define LOST_FRAME_TYPE_FEC (1)
#define LOST_FRAME_TYPE_KCP (2)
//fec,kcp组帧失败丢帧
//type:fec-1,kcp-2......
typedef void (*DecoderRendererNotifyLostFrame)(int video_id, int type);

typedef struct _DECODER_RENDERER_CALLBACKS {
    DecoderRendererSetup setup;
    DecoderRendererCleanup cleanup;
    DecoderRendererSubmitDecodeUnit submitDecodeUnit;
    DecoderRendererNotifyResolution notifyResolution;
    DecoderRendererNotifyRotate notifyRotate;
	DecoderRendererNotifyDirectxRotate notifyDirectXRotate;
	DecoderRendererNotifyLostFrame notifyLostFrame; 
} DECODER_RENDERER_CALLBACKS, *PDECODER_RENDERER_CALLBACKS;

//创建音频播放器通知
typedef int(*AudioRendererInit)();

//清理音频播放器
typedef void(*AudioRendererCleanup)(void);

//音频数据回调
typedef void(*AudioRendererPlaySample)(unsigned char *sampleData, int sampleLength);

typedef struct _AUDIO_RENDERER_CALLBACKS {
    AudioRendererInit init;
    AudioRendererCleanup cleanup;
    AudioRendererPlaySample playSample;
} AUDIO_RENDERER_CALLBACKS, *PAUDIO_RENDERER_CALLBACKS;

//初始化并启动服务
EWRECV_API int EwRecvLibInit(PMAINParam_st pstParam, PDECODER_RENDERER_CALLBACKS drCallbacks,
                             PAUDIO_RENDERER_CALLBACKS arCallbacks);

EWRECV_API int EwRecvLibDestroy();

//停止所有投屏操作
EWRECV_API int MAINInterface_StopAllMedia();

//获取libewrecv版本信息
EWRECV_API char *MAINInterface_GetVersionInfo();

#define HID_EVENT_DOWN (0)
#define HID_EVENT_MOVE (7)
#define HID_EVENT_UP (4)

/*
 * 触摸数据发送接口：最大支持同时传递十个触摸点；
 *
 * buf：触摸数据； bufSize：长度固定为：62
 *
 buf[0]:  固定填：2

 buf[1]~buf[60]: 最多可同时传10个触摸点数据，每个触摸点数据占6字节

 每个触摸点结构：
 buf[point_index + offset_1 + 0]: HID动作类型: HID_EVENT_DOWN=7; HID_EVENT_MOVE=7; HID_EVENT_UP=4;具体参考hid相关协议；
 buf[point_index + offset_1 + 1]: 触摸点编号：1~10；
 buf[point_index + offset_1 + 2]: x_convert & 0xFF
 buf[point_index + offset_1 + 3]: x_convert >> 8 & 0xFF
 buf[point_index + offset_1 + 4]: y_convert & 0xFF
 buf[point_index + offset_1 + 5]: y_convert >> 8 & 0xFF
 其中,point_index取值0~9；
 offset_1为1字节偏移量；
 x_convert = (左上角0坐标参考系的相对坐标值X * 32767)/视频宽度;
 y_convert = (左上角0坐标参考系的相对坐标值Y * 32767)/视频高度;

 buf[61]：此次传递的触摸点个数: 取值1~10；
 * */
//RESERVE
EWRECV_API int MAINInterface_WriteTouchData(int video_id, unsigned char *buf, int bufSize);

/*
 * 通道操作接口：
 * cmd：
STATUS_SHARE_V (3): 开始投屏
STATUS_CLOSE_V (4): 结束投屏（同时结束音频分享）
STATUS_SHARE_A (5): 分享音频
STATUS_CLOSE_A (6): 结束音频
STATUS_FULL_SCREEN (10): 全屏：多分屏时有效
 * */
EWRECV_API int MAINInterface_ConnectOperation(int conn_id, int cmd);

#define BJ_RES_1080P (5)
#define BJ_RES_720P (6)
#define BJ_RES_480P (7)

#define ENCODER_RC_CRF 55
#define ENCODER_RC_ABR 56
#define ENCODER_RC_CQP 57

/*
 * 重置发射端编码参数：
    res 分辨率：   1080-5;720P-6,480p-7
    frame_rate: 帧率，最大支持30帧
    gop：        group of picture； CRF:60; ABR:32768; CQP:60
    rc:         rate control mode；CRF OR ABR
    val:        value;CRF:[0, 51],default:26; ABR:[min max], default:1080P:4096,720p=2048 KBit/s; CQP:[0, 51]:23
//返回值：
//-1-失败，0-成功
 * */
EWRECV_API int MAINInterface_ResetConnectEncoder(int connect_id, int res, int frame_rate, int gop, int rc, int val);

/*
 *I帧请求接口
 * */
EWRECV_API int MAINInterface_ReqPFU(int video_id);

//发送端设置状态回调
typedef void(*MainNotifySenderSetup )(int connect_id, char *stat);

/*
 *发送端设置接口
 crc32lsb: param_json该参数的计算值
 param_json: 希望设置的参数，json格式
 json_len: param_json参数长度，长度超过1000视为异常
 MainNotifySenderSetup: 接口设置结果回调
 //返回值：
//-1-失败，0-成功
 * */
EWRECV_API int MAINInterface_SenderSetup(int connect_id, int crc32lsb,const char *param_json,int json_len,MainNotifySenderSetup cb);


#define SND_TRANS_FEATURE_FEC 			(0x00000001)
#define SND_TRANS_FEATURE_KCP 			(0x00000002)
#define SND_TRANS_FEATURE_FEC_KCP 		(0x00000004)
#define SND_TRANS_FEATURE_AUDIO_FEC 	(0x00000008)
#define SND_TRANS_FEATURE_AUDIO_KCP 	(0x00000010)

/*
 *发送端传输特性获取
 返回值：
 -1:err
 0:udp
 0x00000001:FEC
 0x00000002:KCP
 0x00000004:+FEC+KCP
 0x00000009:VFEC+AFEC
 * */
EWRECV_API int MAINInterface_GetSndTranFeature(int connect_id);

/*
 *当前声音对应通道号获取，对应于上面MAINParam_st中的int nConnCh----最大允许的连接通道数，最大为16
 返回值：
 正常值范围：0~15
 错误或者当前静音：-1
 * */
EWRECV_API int MAINInterface_GetCurAudioCh();

#ifdef __cplusplus
}
#endif
#endif //_H_MAIN_INTERFACE_H
