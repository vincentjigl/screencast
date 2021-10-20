#ifndef _AV_PLAYER_H_
#define _AV_PLAYER_H_
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <media/ICrypto.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/DataSource.h>
#include <media/MediaCodecBuffer.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/MediaCodecList.h>
#include <media/stagefright/MediaDefs.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <ui/DisplayInfo.h>

#include <media/AudioTrack.h>
#include <time.h>

using namespace android;

class VideoBuffer
{
public:
	VideoBuffer() {
	}
	
	void SetBuffer(unsigned char* buffer) {
		mBuffer = buffer;
		mTotalLength = 0;
	}
	
	void AppendBuffer(unsigned char* buffer, int len) {
		memcpy(mBuffer + mTotalLength, buffer, len);
		mTotalLength += len;
	}
	
	void DisposeOneFrame(int len) {
		memmove(mBuffer, mBuffer + len, mTotalLength - len);
		mTotalLength -= len;
	}
	
	int SearchStartCode() {
		int count = 0;
		for (int i = 4; i < mTotalLength; i++) {
			switch(count) {
				case 0:
				case 1:
				case 2:
					if (mBuffer[i] == 0) {
						count++;
					} else {
						count = 0;
					}
				break;
				case 3:
					if (mBuffer[i] == 1) {
						return i - 3;
					} else {
						count = 0;
					}
			}
		}
		
		return 0;
	}
	
	unsigned char* GetBuffer() {
		return mBuffer;
	}

private:
	unsigned char* mBuffer;
	int mTotalLength;
};

class AVPlayer
{
public:
	AVPlayer() {
		mVideoFrameCount = 0;
		mBeginTime = 0;
    }
	
	int InitVideo();
	int FeedOneH264Frame(unsigned char* frame, int size);
	void MakeBackground();

	sp<MediaCodec> mCodec;
	Vector<sp<MediaCodecBuffer> > mBuffers[2];
	sp<SurfaceComposerClient> mComposerClient;
    sp<SurfaceControl> mControl;
	sp<SurfaceControl> mControlBG;
    sp<Surface> mSurface;
	sp<android::ALooper> mLooper;
	sp<AMessage> mFormat;
	
	int mWidth;
	int mHeight;
	bool mRendering;
	
	void CheckIfFormatChange();
	int RenderOneFrame();
	void RenderFrames();
	static void* VideoRenderThread(void* arg);
	
	void Dispose();
	
private:
	int mVideoFrameCount;
	clock_t mBeginTime;
};
#endif
