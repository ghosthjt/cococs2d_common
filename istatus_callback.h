#pragma once
#include <string>

enum 
{
	av_error_publish_video_failed,
	av_error_publish_audio_failed,
	av_error_read_frame_failed,
};

class  runtime_info
{
public:
	long long total_image_send_;
    long long	total_audio_send_;
    long long total_frame_send_;
	bool		reset_;
	runtime_info()
	{
		reset_ = false;
		total_image_send_ = total_audio_send_ = total_frame_send_ = 0;
	}
};

class	 istatus_callback
{
public:
	//视频格式被选定
	virtual void	video_format_selected(int w, int h, int clr_bit) = 0;
	//音频格式被选定
	virtual void	audio_format_selected(int channel, int nsample_per_sec, int bit_per_sample) = 0;
	//回调返回1，系统就不再处理继续处理数据，返回0,系统继续处理
	virtual int		render_video_frame(unsigned char* data, unsigned int len) = 0;
	//回调返回1，系统就不再处理继续处理数据，返回0,系统继续处理
	virtual int		render_audio_frame(unsigned char* data, unsigned int len) = 0;
	//type = 1是libav库的log,此函数在多个线程中被调用。
	virtual void	on_log(int type, std::string fmt, ...) = 0;
	virtual int		on_error(int err) = 0;
	virtual void	clean() = 0;
	virtual ~istatus_callback(){}
};
