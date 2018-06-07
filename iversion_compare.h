#pragma once
#include <vector>
#include <map>
#include "boost/smart_ptr.hpp"
#include "boost/asio.hpp"

#ifdef WIN32
	#ifdef EXPORT_DLL
	#define using_dll __declspec(dllexport)
	#else
	#define using_dll __declspec(dllimport)
	#endif
#else
	#define using_dll
#endif // WIN32

namespace vsp{
enum 
{
	error_noerror,
	error_io_canceled,
	error_move_file_failed,
	error_fileio_failed,
	error_netio_failed,
	error_version_willupdate,
	error_version_uptodate,
	error_should_retry_download,
	error_file_not_exist,
	error_redirect,
};

//状态
enum vsync_st
{
	st_prepare = 0,				//准备状态
	st_pause = 1,				//暂停中
	st_downloading = 2,			//正在下载更新文件
	st_cancel = 3,				//任务取消
	st_download_failed = 4,		//文件下载失败
	st_download_complete = 5,	//文件下载全部完成
	st_unzip = 6,				//正在解压
	st_finished = 7,			//更新已完成
	st_oldversion_remains = 8,
	st_moving_file = 9,
	st_saving_version_config = 10,
	st_verify_version = 11,
};

//更新过程的运行状态
struct	vsync_status
{
	vsync_st	st_;
	//大小 已下载大小, 总大小
	int			progress_, progress_max_;
	//文件 总数, 下载成功数,下载失败数
	int			file_max_,file_succ_, file_failed_;
	//是否需要重启应用程序以完成更新过程.
	int			need_restart_;
	//最后的错误代码
	int			last_error_;
	vsync_status()
	{
		need_restart_ = 0;
		last_error_ = 0;
		st_ = st_prepare;
	}
};

class using_dll iversion_compare
{
public:
	virtual ~iversion_compare(){};
	static iversion_compare*	create_instance();
	virtual std::string	version_info(std::string local_path) = 0;
	virtual void		query_status(vsync_status& st) = 0;

	//检查是否有更新,如果有更新,则调用apply_update更新
	//exe			调用进程的进程名字,用于关闭多开的进程,防止访问冲突
	//local_path	本地要同步的目录
	//remote_path	远程版本目录.
	//返回值: error_xxx 枚举值
	virtual int			check_update(std::string exe, std::string local_path, std::string remote_path)  = 0;
	virtual int			copy_dir(std::string from_dir, std::string dest_dir, std::string& err) = 0;
	//下载并安装更新
	//必须先调用check_update才能调用install_update.
	//返回值: error_xxx 枚举值
	virtual int			install_update()  = 0;
	virtual	bool		is_working() = 0;
	virtual void		clean() = 0;
	virtual int			get_http_data(std::string url, std::string& ret, boost::asio::io_service& ios, bool wait_data = true, std::string header = "") = 0;
protected:
	iversion_compare(){};
};

};