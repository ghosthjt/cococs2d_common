#pragma once
#include "boost/date_time.hpp"
#include "boost/date_time/posix_time/time_formatters.hpp"
#include "boost/thread.hpp"
#include <iostream>
#include <list>
#include <stdarg.h>
#include <string>

template<class out_put_t>
class log_file
{
public:
	enum 
	{
		st_datetime = 1,
		st_breakline = 2,
	};
	out_put_t output_;
	log_file()
	{

	}
	~log_file()
	{
	}
	void	stop_log()
	{
		output_.delFile();
	}

	bool	write_longlog(int bufsize, const char* log_fmt, ...)
	{
#if WIN32
		va_list ap;
		va_start(ap, log_fmt);
		bufsize += 1024;
		char* buff = new char[bufsize];
		memset(buff, 0, bufsize);
		boost::posix_time::ptime p1 = boost::date_time::microsec_clock<boost::posix_time::ptime>::local_time();
		std::string s = boost::posix_time::to_simple_string(p1);
		std::string sfmt = "[" + s + "]:" + log_fmt;	
		sfmt += "\r\n";
		vsnprintf_s(buff, bufsize, bufsize - 1, sfmt.c_str(), ap);
		va_end(ap);
		output_.output(buff, strlen(buff));
		delete [] buff;
#endif
		return true;
	}

	bool	write_log(const char* log_fmt, ...)
	{
#if WIN32
		va_list ap;
		va_start(ap, log_fmt);
		char buff[1024] = {0};
		boost::posix_time::ptime p1 = boost::date_time::microsec_clock<boost::posix_time::ptime>::local_time();
		std::string s = boost::posix_time::to_simple_string(p1);
		std::string sfmt = "[" + s + "]:" + log_fmt;	
		sfmt += "\r\n";
		vsnprintf_s(buff, 1024, 1023, sfmt.c_str(), ap);
		va_end(ap);
		output_.output(buff, strlen(buff));
#endif
		return true;
	}

	bool	write_log(int sytle, const char* log_fmt, ...)
	{
#if WIN32
		va_list ap;
		va_start(ap, log_fmt);
		char buff[1024] = {0};
		std::string sfmt;
		if (sytle & st_datetime){
			boost::posix_time::ptime p1 = boost::date_time::microsec_clock<boost::posix_time::ptime>::local_time();
			std::string s = boost::posix_time::to_simple_string(p1);
			sfmt = "[" + s + "]:" + log_fmt;	
		}
		else{
			sfmt = log_fmt;
		}

		if (sytle & st_breakline){
			sfmt += "\r\n";
		}

		vsnprintf_s(buff, 1024, 1023, sfmt.c_str(), ap);
		va_end(ap);
		output_.output(buff, strlen(buff));
#endif
		return true;
	}
};

struct file_output
{
	std::string fname_;
	void output(char* buff,  unsigned int len)
	{
		FILE* fp = fopen(fname_.c_str(), "a");
		if(fp){
			fwrite(buff, 1, len, fp);
			fclose(fp);
		}
	}
	void delFile()
	{
		remove(fname_.c_str());
	}
};


struct debug_output
{
	void output(char* buff,  unsigned int len)
	{
#ifdef WIN32
		::OutputDebugStringA(buff);
#else
		std::cout<<buff;
#endif
	}
};

struct cout_output
{
	void output(char* buff,  unsigned int len)
	{
		std::cout<<buff;
	}
};

struct cout_and_file_output
{
	std::string fname_;
	void output(char* buff,  unsigned int len)
	{
		FILE* fp = fopen(fname_.c_str(), "a");
		if(fp){
			fwrite(buff, 1, len, fp);
			fclose(fp);
		}
		std::cout<<buff;
	}
};