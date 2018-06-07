#pragma once
#include "boost/smart_ptr.hpp"
#include "boost/system/error_code.hpp"
#include "boost/asio.hpp"

#include <vector>
#include "sending_buffer.h"
#include "json_helper.h"

const int max_msg_size = 1024;

#define GET_CLSID(m) m##_id

#define DECLARE_MSG_CREATE()\
	msg_ptr ret_ptr;\
	switch (cmd)\
{

#define END_MSG_CREATE()\
}\
return ret_ptr

#define REGISTER_CLS_CREATE(cls_)\
	case GET_CLSID(cls_):\
	ret_ptr.reset(new cls_);\
	break

#define MSG_CONSTRUCT(msg)\
msg()\
{\
	head_.cmd_ = msg##_id;\
	init();\
}

#define max_name 64
#define max_guid 64

struct  msg_head
{
	typedef  unsigned short etype;
  unsigned short    len_;
  unsigned short    cmd_;
};

struct  msg_head_int
{
	typedef  unsigned int etype;
	unsigned int    len_;
	unsigned int    cmd_;
};

class native_socket;
template<class T> void swap_byte_order(void* d)
{
#ifndef _NO_SWAP_ORDER
  char* p = (char*)d;
  int s = sizeof(T);
  if (s <= 1) return;
  char t;
  for (int i = 0; i < (s >> 1); i++)
  {
    t = p[i];
    p[i] = p[s - i - 1];
    p[s - i - 1] = t;
  }
#endif
}

struct stream_base : public boost::enable_shared_from_this<stream_base>
{
public:
	stream_base()
	{
		rd_begin_ = NULL;
		rd_buff_len_ = 0;
		rd_fail_ = false;
		wt_begin_ = NULL;
		wt_fail_ = false;
		wt_buff_len_ = 0;
	}

  virtual int				read(boost::property_tree::ptree& jsval) { return 0; }

  virtual int				write(boost::property_tree::ptree& jsval) { return 0; }

  virtual int				write	(char*& data_s, unsigned int  l)
  {
	  wt_begin_ = (char*)data_s;
	  wt_buff_len_ = l;
	  wt_fail_ = false;
	  return 0;
  }
  virtual int				read	(const char*& data_s, unsigned int l)
  {
    rd_begin_ = (char*)data_s;
    rd_buff_len_ = l;
		rd_fail_ = false;
    return 0;
  }

  virtual ~stream_base(){}
  template<class T>
  T				read_data(const char*&data_s, bool swap_order = true)
  {
    if((rd_buff_len_ - ((char*)data_s - rd_begin_)) >= sizeof(T)){
			if (swap_order)	{
				swap_byte_order<T>((char*)data_s);
			}

			T t = 0;
			memcpy((void*)&t, data_s, sizeof(T));
      data_s += sizeof(T);
      return t;
    }
    else{
      rd_fail_ = true;
      return T(0);
    }
  }
  template<class T>
  void			read_data(T& t, const char*&data_s, bool swap_order = true)
  {
    if((rd_buff_len_ - ((char*)data_s - rd_begin_)) >= sizeof(T)){
			if (swap_order)	{
				swap_byte_order<T>((char*)data_s);
			}
			memcpy((void*)&t, data_s, sizeof(T));
      data_s += sizeof(T);
    }
    else {
      rd_fail_ = true;
    }
  }
  template<class T>
  void			read_data(T* v, unsigned int element_count, const char*&data_s, bool swap_order = false)
  {
		if(rd_buff_len_ - (data_s - rd_begin_) < sizeof(T) * element_count){
			rd_fail_ = true;
			return;
		}
    T* vv = (T*)v;
    for (unsigned int i = 0 ; i < element_count; i++)
    {	
			if (swap_order)	{
				swap_byte_order<T>((char*)data_s);
			}
			T t = 0;
			memcpy((void*)&t, data_s, sizeof(T));
			data_s += sizeof(T);
			*vv = t;
			//防止越界
			if((i + 1) < element_count)
				vv++;
    }
  }

	template<class T>
	void			read_data(std::vector<T>& v, const char*& data_s, unsigned int element_count, bool swap_order = false)
	{
		if(wt_buff_len_ - (data_s - wt_begin_) < sizeof(T) * v.size()){
			wt_fail_ = true;
			return;
		}

		for (unsigned int i = 0 ; i < element_count; i++)
		{	
			if (swap_order)	{
				swap_byte_order<T>((char*)data_s);
			}
			T t = 0;
			memcpy((void*)&t, data_s, sizeof(T));
			data_s += sizeof(T);
			v.push_back(t);
		}
	}

  template<class T>
  void			write_data(T v, char*& data_s, bool swap_order = true)
  {
		if(wt_buff_len_ - (data_s - wt_begin_) < sizeof(T)){
			wt_fail_ = true;
			return;
		}
		if (swap_order)	{
				swap_byte_order<T>(&v);
		}
     
    memcpy(data_s, &v, sizeof(T));
    data_s += sizeof(T);
  }
  template<class T>
  void			write_data(T* v, unsigned int element_count, char*& data_s, bool swap_order = false)
  {
		if(wt_buff_len_ - (data_s - wt_begin_) < sizeof(T) * element_count){
			wt_fail_ = true;
			return;
		}
    T* vv = (T*)v;
    for (unsigned int i = 0 ; i < element_count; i++)
    {
      T t = *vv;
      if (swap_order)	{
        swap_byte_order<T>(&t);
      }
      memcpy(data_s, &t, sizeof(T));
      data_s += sizeof(T);
			//防止越界
			if((i + 1) < element_count)
				vv++;
    }
  }
	template<class T>
	void			write_data(std::vector<T>& v, char*& data_s, bool swap_order = false)
	{
		if(wt_buff_len_ - (data_s - wt_begin_) < sizeof(T) * v.size()){
			wt_fail_ = true;
			return;
		}
		for (unsigned int i = 0 ; i < v.size(); i++)
		{
			T t = v[i];
			if (swap_order)	{
				swap_byte_order<T>(&t);
			}
			memcpy(data_s, &t, sizeof(T));
			data_s += sizeof(T);
		}
	}
	virtual bool				read_successful()	{return !rd_fail_;}
	virtual bool				write_successful(){return !wt_fail_;}
private:
	char*				rd_begin_;	//开始读取位置,用来检测越界读
	unsigned int		rd_buff_len_;
	bool				rd_fail_;
	char*				wt_begin_;	//开始读取位置,用来检测越界读
	unsigned int		wt_buff_len_;
	bool				wt_fail_;
};

template<> void	stream_base::read_data<char>(char* v, unsigned int element_count, const char*&data_s, bool swap_order);
template<> void	stream_base::write_data<char>(char* v, unsigned int element_count, char*& data_s, bool swap_order);

class msg_base : public stream_base
{
public:
	msg_head		head_;
	boost::weak_ptr<native_socket> from_sock_;
	int					read(const char*& data_s, unsigned int l)
	{
		stream_base::read(data_s, l);
		read_data(head_.len_, data_s);
		read_data(head_.cmd_, data_s);
		return 0;
	}

	int write(char*& data_s, unsigned int l)
	{
		stream_base::write(data_s, l);
		write_data(head_.len_, data_s);
		write_data(head_.cmd_, data_s);
		return 0;
	}
	virtual int				read(boost::property_tree::ptree& jsval) { return 0; }

	virtual int				write(boost::property_tree::ptree& jsval) { return 0; }
	virtual	int handle_this() {return 0;};
	void				init()
	{
		head_.len_ = 0;
	}
};

class msg_common_reply: public msg_base
{
public:
	int				rp_cmd_;
	int				err_;
	char			des_[128];
	msg_common_reply()
	{
		head_.cmd_ = 1001;
		memset(des_, 0, 128);
	}

	int			read(boost::property_tree::ptree& data_s)
	{
		msg_base::read(data_s);
		read_jvalue(rp_cmd_, data_s);
		read_jvalue(err_, data_s);
		read_jstring(des_, 128, data_s);
		return 0;
	}

	int			write(boost::property_tree::ptree& data_s)
	{
		msg_base::write(data_s);
		write_jvalue(rp_cmd_, data_s);
		write_jvalue(err_, data_s);
		write_jvalue(des_, data_s);
		return 0;
	}

	int			read(const char*& data_s, unsigned int l)
	{
		msg_base::read(data_s, l);
		read_data(rp_cmd_, data_s);
		read_data(err_, data_s);
		read_data<char>(des_, 128, data_s);
		return 0;
	}

	int			write(char*& data_s, unsigned int l)
	{
		msg_base::write(data_s, l);
		write_data(rp_cmd_, data_s);
		write_data(err_, data_s);
		write_data<char>(des_, 128, data_s);
		return 0;
	}


	int			handle_this();
};

class msg_json_form : public msg_base
{
public:
	unsigned short  sub_cmd_;
	//重载缓冲分配
	stream_buffer	alloc_io_buffer();

	void	set_content(std::string json_dat);

	std::string content()
	{
		return content_;
	}

	msg_json_form()
	{
		head_.cmd_ = 0xCCCC;
	}

	int			read(const char*& data_s, unsigned int l);

	int			write(char*& data_s, unsigned int l);

private:
	unsigned short  is_compressed_;		//尽量对齐字节为4的倍数
	std::string     content_;
};

static stream_buffer build_send_stream(msg_base* msg)
{
	typedef short etype;
	char* pstrm = new char[max_msg_size];
	char* pc = pstrm;
	boost::shared_array<char> ret(pstrm);
	msg->write(pstrm, max_msg_size);
	etype len = pstrm - pc;
	swap_byte_order<etype>(&len);
	*(etype*) pc = len;
	return stream_buffer(ret, pstrm - pc, max_msg_size);
}

#ifdef USE_JSON_PROTOCOL
#define send_protocol   send_json
#else
#define send_protocol   send_msg
#endif

template<class remote_t, class msg_t>
int			send_json(remote_t psock, msg_t& msg)
{
	msg_json_form ack;
	ack.sub_cmd_ = msg.head_.cmd_;
	std::string str = json_msg_helper::to_json(&msg);
	ack.set_content(str);
	return send_msg(psock, ack);
}

template<class socket_t, class msg_base_t>
int			send_msg(socket_t psock, msg_base_t& msg)
{
	if (!psock.get())
		return -1;

	stream_buffer strm = build_send_stream(&msg);
	return psock->send_data(strm);
}

template<class msg_creator_t>
boost::shared_ptr<msg_base> read_msg_from_json(boost::shared_ptr<msg_json_form> msg_json, msg_creator_t creator)
{
	boost::shared_ptr<msg_base> msg = creator(msg_json->sub_cmd_);
	if (msg != nullptr && !msg_json->content().empty()) {
		boost::property_tree::ptree jsn_obj;
		std::stringstream sstr;
		sstr << msg_json->content();
		try {
			boost::property_tree::read_json(sstr, jsn_obj);
			msg->read(jsn_obj);
			return msg;
		}
		catch (boost::property_tree::json_parser_error e) {
			std::cout << "json msg parse error: " << e.message() << std::endl;
		}
	}
	else if (!msg) {
		std::cout << "can't create msg, cmd = " << msg_json->sub_cmd_ << std::endl;
	}
	return msg_json;
}

template<class msg_creator_t>
boost::shared_ptr<msg_base>	 read_msg(char * c, unsigned short n, msg_creator_t creator, bool& got_msg)
{
	got_msg = true;
	msg_head head;
	head = *(msg_head*)c;
	head.cmd_ = ntohs(head.cmd_);

	if (head.cmd_ == 0xCCCC) {
		boost::shared_ptr<msg_json_form> msg_json(new msg_json_form());
		msg_json->read((const char*&)c, n);
		if (msg_json->read_successful()) {
			return read_msg_from_json(msg_json, creator);
		}
		else {
			std::cout << "json msg read error. " << std::endl;
		}
	}
	else {
		boost::shared_ptr<msg_base> msg = creator(head.cmd_);
		if (msg.get()) {
			msg->read((const char*&)c, n);
			if (msg->read_successful()) {
				return msg;
			}
			else {
				std::cout << "msg read failed, cmd = " << head.cmd_ << std::endl;
			}
		}
		else {
			std::cout << "can't create msg, cmd = " << head.cmd_ << std::endl;
		}
	}
	return boost::shared_ptr<msg_base>();
}

template<class remote_t, class msg_creator_t>
boost::shared_ptr<msg_base>		pickup_msg_from_socket(remote_t psock, msg_creator_t creator, bool& got_msg)
{
	unsigned short n = 0;
	if (psock->pickup_data(&n, 2, false) > 0) {
		n = ntohs(n);
		char * c = new char[n];
		boost::shared_array<char> dat(c);
		if (psock->pickup_data(c, n, true) == 0) return boost::shared_ptr<msg_base>();
		return read_msg(c, n, creator, got_msg);
	}
	return boost::shared_ptr<msg_base>();
}
