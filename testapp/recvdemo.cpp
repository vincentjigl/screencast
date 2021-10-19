/* coding:utf-8 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

extern void crc32_lsb_init(void);
extern unsigned int crc32_lsb(unsigned int crc, const unsigned char *buffer, unsigned int size)  ;

#ifdef __cplusplus
}
#endif

#ifdef ANDROID
#include <unistd.h>
#include <string>
#include "../mcdec/avplayer.h"
#include "nlohmann/crc32.h"
//#include "nlohmann/json.hpp"
//using json = nlohmann::json;
#pragma ("ANDROID is defined")
#endif

#include "MainInterface.h"

#define STATUS_SHARE_V (3)
#define  STATUS_CLOSE_V  (4)
#define  STATUS_SHARE_A  (5)
#define  STATUS_CLOSE_A  (6)
#define  STATUS_FULL_SCREEN  (10)


int thread_exit = 0;
int g_isAudioInit = 0;

#if defined(WIN32) || defined(ANDROID)
int g_delaysend = 5;
#endif

FILE *g_audio_fp = NULL;
FILE *g_video_fp[4] = {NULL};
AVPlayer *avplayer = NULL;

//文件保存的pcm数据，可以通过pcm2wave.c提供的函数转为wave文件进行播放
int write_audio_to_file(unsigned char *buf, int len) {

    if (g_audio_fp == NULL) {
        printf("APP::OnFirstAudio\n");
        g_audio_fp = fopen("44.1k_s16le.pcm", "wb");
    }

    fwrite(buf, len, 1, g_audio_fp);
    fflush(g_audio_fp);
    return 1;
}
//文件保存的H264数据，可以使用h264分析器打开或VLC播放器播放
int write_video_to_file(int video_id, unsigned char *buf, int len) {

    if (g_video_fp[video_id] == NULL) {
        return -1;
    }

	avplayer->FeedOneH264Frame(buf, len);
    fwrite(buf, len, 1, g_video_fp[video_id]);
    fflush(g_video_fp[video_id]);
    return 1;
}

void OnSenderSetupCallback(int id,char *info)
{
    printf("SenderSetup Callback 0x%x::%s\n\n", id,info);
}

void OnLogCallback(int level, const char *format, ...) {

    if (level > 5) {
        return;
    }
    static char line[1024] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(line, sizeof(line), format, arg_ptr);
    va_end(arg_ptr);

    printf("EwRecv::%s", line);

    return;

}

int OnWriteVideoData(int video_id, unsigned char *buf, int len) {

    printf("OnWriteVideoData: video_id=%d, size=%d \n", video_id, len);
    write_video_to_file(video_id, buf, len);
    return 0;
}


void OnWriteAudioData(unsigned char *buf, int bufSize) {
    printf("OnWriteAudioData: size=%d \n", bufSize);
    write_audio_to_file(buf, bufSize);

    return;
}


char *getMirroringStatusDscr(int stat) {
    switch (stat) {
        case STATUS_SHARE_V:
            return "START_VIDEO";
        case STATUS_CLOSE_V:
            return "STOP_VIDEO";
        case STATUS_SHARE_A:
            return "START_AUDIO";
        case STATUS_CLOSE_A:
            return "STOP_AUDIO";
        default:
            return "UNK_STATE";
    }
}

char *getDeviceTypeDscr(int type) {
    switch (type) {
        case 0:
            return "CONNECT_DONGLE";
        case 1:
            return "CONNECT_ANDROID";
        /*case 2:
            return "CONNECT_AIRPLAY";*/
        case 4:
            return "CONNECT_WIN";
        default:
            return "UNK_CONNECT";
    }
}

int constructTestTouchData(unsigned char *buf, int hid_action, int x_poit, int y_poit, int width, int height) {
    if (buf == NULL) { return -1; }

    int x_convert = (x_poit * 32767) / width;
    int y_convert = (y_poit * 32767) / height;
    int point_index = 0;
    int offset_1 = 1;

    buf[0] = 2;
    buf[61] = 1; //触摸点数据的个数
    //第一个触摸点数据，其他触摸点类型：
    buf[point_index + offset_1 + 0] = hid_action;//HID动作类型
    buf[point_index + offset_1 + 1] = 1; //触摸点编码
    buf[point_index + offset_1 + 2] = (unsigned char) (x_convert & 0xFF);
    buf[point_index + offset_1 + 3] = (unsigned char) (x_convert >> 8 & 0xFF);
    buf[point_index + offset_1 + 4] = (unsigned char) (y_convert & 0xFF);
    buf[point_index + offset_1 + 5] = (unsigned char) (y_convert >> 8 & 0xFF);
    return 0;
}


void OnMirroringStatus(int conn_id, int video_id, int status) {
    printf("APP::OnMirroringStatus: conn_id=%d, video_id=%d, %s \n", conn_id, video_id, getMirroringStatusDscr(status));



    if (status == STATUS_SHARE_V) {

        //支持多路投屏时，用户需要记录和关联：连接通道conn_id和投屏通道video_id

        //模拟用户的操作
        //修改连接通道的分辨率
        //MAINInterface_ResetConnectEncoder(conn_id, BJ_RES_1080P, 10, 2 * 10, ENCODER_RC_ABR, 1500);
        //发送I帧请求
        //MAINInterface_ReqPFU(video_id);
        //通道操作：例如全屏，踢出或控制dongle投屏等
        //MAINInterface_ConnectOperation(conn_id, STATUS_FULL_SCREEN);
		
        //触摸回传测试：模拟屏幕触摸输入，从点(50,50)滑动到点(100,100)
#if 0
        unsigned char hid_touch_data[62];
        constructTestTouchData(hid_touch_data, HID_EVENT_DOWN, 50, 50, 1280, 720);
        MAINInterface_WriteTouchData(video_id, hid_touch_data, 62);

        constructTestTouchData(hid_touch_data, HID_EVENT_MOVE, 50, 50, 1280, 720);
        MAINInterface_WriteTouchData(video_id, hid_touch_data, 62);

        constructTestTouchData(hid_touch_data, HID_EVENT_MOVE, 100, 100, 1280, 720);
        MAINInterface_WriteTouchData(video_id, hid_touch_data, 62);

        constructTestTouchData(hid_touch_data, HID_EVENT_UP, 100, 100, 1280, 720);
        MAINInterface_WriteTouchData(video_id, hid_touch_data, 62);
#endif
    }

    return;
}

int OnVideoPlayerSetup(int video_id, int conn_id) {

    printf("APP::OnVideoPlayerSetup: video_id=%d, conn_id=%d\n", video_id, conn_id);
    if (g_video_fp[video_id] == NULL) {

        char filename[32] = {0};
        sprintf(filename, "ew_video-chn%d.h264", video_id);
        g_video_fp[video_id] = fopen(filename, "wb");
    }

    return 0;
}

void OnVideoPlayerCleanUp(int video_id) {
    printf("APP::OnVideoPlayerCleanUp: video_id=%d\n", video_id);
#ifndef WIN32
	if (g_video_fp[video_id] != NULL) {
        fclose(g_video_fp[video_id]);
        g_video_fp[video_id] = NULL;
    }
#endif
    return;
}


void OnNotifyResolution(int video_id, int width, int height) {

    printf("APP::OnNotifyResolution: video_id=%d, %d x %d \n", video_id, width, height);

    return;
}

void OnNotifyRotate(int video_id, int rotate) {
    printf("APP::OnNotifyRotate: video_id=%d, rotate=%d\n", video_id, rotate);
    return;
}

void OnNotifyDirectXRotate(int video_id, int rotate) {
    printf("APP::OnNotifyDirectXRotate: video_id=%d, rotate=%d\n", video_id, rotate);
    return;
}


void OnConnState(int conn_id, int stat, int total_conn_num, char *pIpAddr) {
    printf("APP::OnConnState: conn_id=%d, addr=%s, stat:%s, total:%d\n",
           conn_id,
           pIpAddr != NULL ? pIpAddr : "...",
           (stat == 0) ? "Offline" : "Online",
           total_conn_num);

    if (stat == 1) 
	{
        //MAINInterface_ResetConnectEncoder(conn_id, BJ_RES_720P, 30, 2 * 30, ENCODER_RC_CRF, 26);
        //MAINInterface_ResetConnectEncoder(conn_id, BJ_RES_1080P, 10, 2 * 10, ENCODER_RC_ABR, 1500);
    }

    return;
}

void OnGotDeviceName(int conn_id, int type, char *pDevName) {
    if (pDevName == NULL) { return; }

    printf("APP::OnGotDeviceName:conn_id=%d, pDevName:%s, type:%s\n", conn_id, pDevName, getDeviceTypeDscr(type));

	int feature = MAINInterface_GetSndTranFeature(conn_id);
	//if(SND_TRANS_FEATURE_FEC != feature)
	if(SND_TRANS_FEATURE_KCP != feature)
	//if(SND_TRANS_FEATURE_FEC_KCP != feature)	
	{
		MAINInterface_ResetConnectEncoder(conn_id, BJ_RES_1080P, 10, 2 * 10, ENCODER_RC_ABR, 1500);
		
#if defined(WIN32)		
		json k;
				
		k["getinfo"] = {"buttonid"};
		//k["video"]["rotate"] = 0;
		//k["video"]["mirror"] = 2;
		//k["transport"]["feature"] = "fec";
		//k["transport"]["feature"] = "kcp";
		//k["transport"]["feature"] = "+fec+kcp";
		//k["transport"]["on"] = 1;
		std::string param_json = k.dump();
#elif defined (ANDROID)
		//std::string param_json = "{\"video\":{\"rotate\":0,\"mirror\":2}}";			
		std::string param_json = "{\"video\":{\"rotate\":0,\"mirror\":2};\"getinfo\":[\"buttonid\"]}";
#endif
		printf("------> param_json %s,len %lu\n", param_json.c_str(),param_json.length());			
		crc32_lsb_init();
		unsigned int calc32 = crc32_lsb(0xFFFFFFFF,(const unsigned char *)param_json.c_str(),param_json.length());
		printf("------> crc32lsb:0x%x\n", calc32);			
		MAINInterface_SenderSetup(conn_id, calc32, param_json.c_str(),param_json.length(), OnSenderSetupCallback);

	}
    return;
}

void OnResetEncoderResult(int conn_id, int result) {
    printf("APP::OnResetEncoderResult:conn_id=%d, result:%d\n", conn_id, result);

    return;
}


int audioInit() {
    if (!g_isAudioInit) {
        printf("APP::audioInit init\n");
        g_isAudioInit = 1;
    }
    return 0;
}

void audioCleanup(void) {
    printf("APP::audioInit clean up\n");
    return;
}

void stopEwService() {
    EwRecvLibDestroy();

    if (g_audio_fp != NULL) {
        fclose(g_audio_fp);
        g_audio_fp = NULL;
    }
    int i;
    for (i = 0; i < 4; i++) {
        if (g_video_fp[i] != NULL) {
            fclose(g_video_fp[i]);
            g_video_fp[i] = NULL;
        }
    }
    printf("APP::stopEwService \n");
}

int startEwService() {

    printf("APP::startEwService: %s \n", MAINInterface_GetVersionInfo());

    MAINParam_st MainCallbacks = {
            .nMediaCh = 1, //<=4
            .nConnCh = 16, //<=16
            .nTcpPort= 10000,
            .notifyMirroringStat = OnMirroringStatus,
            .notifyConnStat = OnConnState,
            .notifyConnHostInfo = OnGotDeviceName,
            .notifyResetEncoderResult = OnResetEncoderResult,
            .writeLog  =  OnLogCallback,
    };

    DECODER_RENDERER_CALLBACKS _VideoCallbacks = {
            .setup = OnVideoPlayerSetup,
            .cleanup = OnVideoPlayerCleanUp,
            .submitDecodeUnit = OnWriteVideoData,
            .notifyResolution = OnNotifyResolution,
            .notifyRotate= OnNotifyRotate,
			.notifyDirectXRotate = OnNotifyDirectXRotate,
    };

    AUDIO_RENDERER_CALLBACKS _AudioCallbacks = {
            .init = audioInit,
            .cleanup = audioCleanup,
            .playSample = OnWriteAudioData,
    };

    //start service
    int ret = EwRecvLibInit(&MainCallbacks, &_VideoCallbacks, &_AudioCallbacks);

    return ret;
}

void sig_action(int sig) {

    //printf("APP::process exit!!!\n");
    fflush(stdout);

    MAINInterface_StopAllMedia();
	
	thread_exit = 1;
    //stopEwService();
    //exit(0);
}


//avahi-publish -s  ubuntu_recv_sdk  _eshow._tcp 5353
int main(int argc, char *argv[]) {

    signal(SIGINT, sig_action);
    signal(SIGTERM, sig_action);

    startEwService();

    const sp<IBinder> display = SurfaceComposerClient::getInternalDisplayToken();
    CHECK(display != nullptr);
    
    DisplayInfo info;
    CHECK_EQ(SurfaceComposerClient::getDisplayInfo(display, &info), NO_ERROR);
    int width = info.w;
    int height = info.h;
	int offsetx =0;
	int offsety =0;
	char path[128];// "/data/test.264";


    std::cout<<"w="<<width<<", h="<<height<<" , x="<<offsetx<<" , y="<<offsety<<", path ="<<path<<std::endl;
	avplayer = new AVPlayer(width, height, offsetx, offsety, path);
	avplayer->InitVideo();

    while (thread_exit == 0) {

		usleep(1000*1000);
		g_delaysend--;
		if(0 == g_delaysend)
		{			
			/*std::string param_json = "{\"video\":{\"rotate\":0,\"mirror\":2}}";			
			printf("------> param_json %s,len %d\n", param_json.c_str(),param_json.length());			
			crc32_lsb_init();
			unsigned int calc32 = crc32_lsb(0xFFFFFFFF,(const unsigned char *)param_json.c_str(),param_json.length());
			printf("------> crc32lsb:0x%x\n", calc32);			
			MAINInterface_SenderSetup(0, calc32, param_json.c_str(),param_json.length(), OnSenderSetupCallback);*/

			std::string param_json = "{\"getinfo\":[\"buttonid\"]}";			
			printf("------> param_json %s,len %lu\n", param_json.c_str(),param_json.length());			
			crc32_lsb_init();
			unsigned int calc32 = crc32_lsb(0xFFFFFFFF,(const unsigned char *)param_json.c_str(),param_json.length());
			printf("------> crc32lsb:0x%x\n", calc32);			
			MAINInterface_SenderSetup(0, calc32, param_json.c_str(),param_json.length(), OnSenderSetupCallback);

						
			//g_delaysend = 5;
		}
    }
    stopEwService();

    printf("APP exit!!!\n");
	
    return 0;
}
