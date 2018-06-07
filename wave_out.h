#pragma once
#include <WTypes.h>
#include <vector>
#include "boost/thread.hpp"
#include "boost/atomic.hpp"

#include "mmsystem.h"

typedef void(*done_cb)(int samples_played);
class wave_out
{
public:

	explicit wave_out()
	{
		hwav_out = NULL;
		is_opened = false;
	}

	bool		open_device(int channel, int nsample_per_sec, int bit_per_sample, done_cb cb);
	bool		play_sound(char* pbuff, int len);
	void		close();
	void		free_later(PWAVEHDR hdr);
	done_cb cb_;	
protected:
	HWAVEOUT hwav_out;
	std::list<PWAVEHDR> free_lst_;
	bool	is_opened;
	bool	is_buffering_;

	WAVEFORMATEX fmt;
	void		clean();
	void		free_one_complete();
};