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
	//��Ƶ��ʽ��ѡ��
	virtual void	video_format_selected(int w, int h, int clr_bit) = 0;
	//��Ƶ��ʽ��ѡ��
	virtual void	audio_format_selected(int channel, int nsample_per_sec, int bit_per_sample) = 0;
	//�ص�����1��ϵͳ�Ͳ��ٴ�������������ݣ�����0,ϵͳ��������
	virtual int		render_video_frame(unsigned char* data, unsigned int len) = 0;
	//�ص�����1��ϵͳ�Ͳ��ٴ�������������ݣ�����0,ϵͳ��������
	virtual int		render_audio_frame(unsigned char* data, unsigned int len) = 0;
	//type = 1��libav���log,�˺����ڶ���߳��б����á�
	virtual void	on_log(int type, std::string fmt, ...) = 0;
	virtual int		on_error(int err) = 0;
	virtual void	clean() = 0;
	virtual ~istatus_callback(){}
};
