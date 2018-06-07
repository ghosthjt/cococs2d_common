#include "wave_out.h"
#include "boost/atomic.hpp"

boost::mutex mut_;
//这是个声卡线程函数,需要同步
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	if (uMsg == WOM_DONE)	{
		LPWAVEHDR head = (LPWAVEHDR) dwParam1;
		wave_out* pthis = (wave_out*) dwInstance;
		if(pthis->cb_){
			pthis->cb_(1);
		}
		pthis->free_later(head);
	}
}

//在所有可能的工作线程中调用
bool wave_out::open_device(int channel, int nsample_per_sec, int bit_per_sample, done_cb cb)
{
	cb_ = cb;
	fmt.wBitsPerSample = bit_per_sample;
	fmt.nBlockAlign = bit_per_sample / 8 * channel;
	fmt.nChannels = channel;
	fmt.nSamplesPerSec = nsample_per_sec;
	fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
	fmt.wFormatTag = 1;
	fmt.cbSize = 0;
	MMRESULT ret = waveOutOpen(&hwav_out, WAVE_MAPPER, &fmt, (DWORD_PTR)waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (ret == 0){
		is_opened = true;
	}
	is_buffering_ = true;
	return ret == 0;
}

void	delete_wav_header(HWAVEOUT wo, LPWAVEHDR header)
{
	waveOutUnprepareHeader(wo, header, sizeof(WAVEHDR));
	delete [] header->lpData;
}
//在所有可能的工作线程中调用
bool wave_out::play_sound(char* pbuff, int len)
{
	free_one_complete();

	char* buff = new char[len];
	memcpy(buff, pbuff, len);
	WAVEHDR* hdr = new WAVEHDR();
	hdr->lpData = buff;
	hdr->dwBufferLength = len;
	hdr->dwFlags = WHDR_PREPARED;
	hdr->dwBytesRecorded = 0;
	hdr->dwUser = (DWORD_PTR)0;
	hdr->reserved = 0;
	hdr->dwLoops = 1;
	hdr->lpNext = NULL;
	MMRESULT ret = waveOutPrepareHeader(hwav_out, hdr, sizeof(WAVEHDR));
	if (ret == MMSYSERR_NOERROR ){
		ret = waveOutWrite(hwav_out, hdr, sizeof(WAVEHDR));
	}

	if (ret != MMSYSERR_NOERROR){
		delete [] buff;
		delete hdr;
	}

	return ret == MMSYSERR_NOERROR;
}

//在所有可能的工作线程中调用
void wave_out::close()
{
	waveOutReset(hwav_out);
	clean();
	if (hwav_out){
		waveOutClose(hwav_out);
		hwav_out = nullptr;
	}
}

void wave_out::free_later(PWAVEHDR hdr)
{
	boost::lock_guard<boost::mutex> lk(mut_);
	free_lst_.push_back(hdr);
}

void wave_out::clean()
{
	boost::lock_guard<boost::mutex> lk(mut_);
	auto it = free_lst_.begin();
	while (it != free_lst_.end())
	{
		delete_wav_header(hwav_out, *it);
		it++;
	}
	free_lst_.clear();
}

void wave_out::free_one_complete()
{
	boost::lock_guard<boost::mutex> lk(mut_);
	if (!free_lst_.empty()){
		auto it = free_lst_.begin();
		delete_wav_header(hwav_out, *it);
		free_lst_.pop_front();
	}
}
