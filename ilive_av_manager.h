/************************************************************************/
/* ����Ƶģ��
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
	//���п��ò�׽�豸,dv_type = 0 ��Ƶ =1
	virtual void	available_devices(int dv_type, std::vector<device_identify>& ret) = 0;
	//ѡ����׽�豸����Ƶ����Ƶ,�ڿ�ʼ����ǰ
	virtual void	select_devices(std::wstring vid, std::wstring aid) = 0;
	//������Ƶ��׽����,�ڿ�ʼ����ǰ�趨,(0-100)
	virtual void	adjust_video_quality(float quality) = 0;
	//����״̬�ص�,�ڿ�ʼ����ǰ�趨
	virtual	void	set_callback(istatus_callback* cb) = 0;
	//����ƫ�÷ֱ��ʺ�fps,�ڿ�ʼ����ǰ�趨
	virtual void	set_video_prefer(int w, int h, int ctrl_fps) = 0;
	//����ƫ����Ƶ����,�ڿ�ʼ����ǰ�趨
	virtual void	set_audio_prefer(int sample_per_sec, int wbit_per_sample, int channel, int bitrate) = 0;
	//ѡ���豸��ʼ������vid��Ƶ�豸��aid��Ƶ�豸
	virtual int		host(const char* url) = 0;
	//ֹͣ����
	virtual void	stop() = 0;	
	//Ԥ��
	virtual void	preview() = 0;
	//ֹͣԤ��
	virtual	void	stop_preview() = 0;
	virtual void	enable_audio(bool enable) = 0;
	virtual void  get_runtime_info(runtime_info& inf) = 0;
	virtual ~i_live_av_capture(){};
};

DLL_EXPORT i_live_av_capture*  create_cap_manager();
DLL_EXPORT void  free_cap_manager(i_live_av_capture* ins);



