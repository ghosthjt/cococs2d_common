/************************************************************************/
/* 音视频模块
/* Develop By HONGJIANTAO
/* LastVersion 2015-5-13
/************************************************************************/

#pragma once

#ifdef WIN32
#ifdef EXPORT_LIVEAV
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT 
#endif

#include "istatus_callback.h"

#include <vector>

class	 device_identify
{
public:
	std::wstring	dev_desc;
	std::wstring	dev_id_;
};

class DLL_EXPORT i_live_av_capture
{
public:
	//所有可用捕捉设备,dv_type = 0 视频 =1
	virtual void	available_devices(int dv_type, std::vector<device_identify>& ret) = 0;
	//选定捕捉设备，视频，音频,在开始主播前
	virtual void	select_devices(std::wstring vid, std::wstring aid) = 0;
	//调整视频捕捉质量,在开始主播前设定,(0-100)
	virtual void	adjust_video_quality(float quality) = 0;
	//设置状态回调,在开始主播前设定
	virtual	void	set_callback(istatus_callback* cb) = 0;
	//设置偏好分辨率和fps,在开始主播前设定
	virtual void	set_video_prefer(int w, int h, int ctrl_fps) = 0;
	//设置偏号音频参数,在开始主播前设定
	virtual void	set_audio_prefer(int sample_per_sec, int wbit_per_sample, int channel, int bitrate) = 0;
	//选择设备开始主播，vid视频设备，aid音频设备
	virtual int		host(const char* url) = 0;
	//停止主播
	virtual void	stop() = 0;	
	//预览
	virtual void	preview() = 0;
	//停止预览
	virtual	void	stop_preview() = 0;
	virtual void	enable_audio(bool enable) = 0;
	virtual void  get_runtime_info(runtime_info& inf) = 0;
	virtual ~i_live_av_capture(){};
};

DLL_EXPORT i_live_av_capture*  create_cap_manager();
DLL_EXPORT void  free_cap_manager(i_live_av_capture* ins);



