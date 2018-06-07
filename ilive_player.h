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

class i_live_player
{
public:
	virtual	void	set_callback(istatus_callback* cb) = 0;
	virtual int		play(const char* url) = 0;
	virtual int		stop() = 0;
	virtual int		check_status() = 0;
	virtual void	audio_samples_played(int played) = 0;
	virtual void  get_runtime_info(runtime_info& inf) = 0;
	virtual void	mute(bool mut) = 0;
	virtual ~i_live_player(){};
};

DLL_EXPORT i_live_player*  create_avplayer();
DLL_EXPORT void  free_avplayer(i_live_player* ins);