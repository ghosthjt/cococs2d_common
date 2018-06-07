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

//״̬
enum vsync_st
{
	st_prepare = 0,				//׼��״̬
	st_pause = 1,				//��ͣ��
	st_downloading = 2,			//�������ظ����ļ�
	st_cancel = 3,				//����ȡ��
	st_download_failed = 4,		//�ļ�����ʧ��
	st_download_complete = 5,	//�ļ�����ȫ�����
	st_unzip = 6,				//���ڽ�ѹ
	st_finished = 7,			//���������
	st_oldversion_remains = 8,
	st_moving_file = 9,
	st_saving_version_config = 10,
	st_verify_version = 11,
};

//���¹��̵�����״̬
struct	vsync_status
{
	vsync_st	st_;
	//��С �����ش�С, �ܴ�С
	int			progress_, progress_max_;
	//�ļ� ����, ���سɹ���,����ʧ����
	int			file_max_,file_succ_, file_failed_;
	//�Ƿ���Ҫ����Ӧ�ó�������ɸ��¹���.
	int			need_restart_;
	//���Ĵ������
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

	//����Ƿ��и���,����и���,�����apply_update����
	//exe			���ý��̵Ľ�������,���ڹرն࿪�Ľ���,��ֹ���ʳ�ͻ
	//local_path	����Ҫͬ����Ŀ¼
	//remote_path	Զ�̰汾Ŀ¼.
	//����ֵ: error_xxx ö��ֵ
	virtual int			check_update(std::string exe, std::string local_path, std::string remote_path)  = 0;
	virtual int			copy_dir(std::string from_dir, std::string dest_dir, std::string& err) = 0;
	//���ز���װ����
	//�����ȵ���check_update���ܵ���install_update.
	//����ֵ: error_xxx ö��ֵ
	virtual int			install_update()  = 0;
	virtual	bool		is_working() = 0;
	virtual void		clean() = 0;
	virtual int			get_http_data(std::string url, std::string& ret, boost::asio::io_service& ios, bool wait_data = true, std::string header = "") = 0;
protected:
	iversion_compare(){};
};

};