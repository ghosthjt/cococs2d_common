#pragma once
#include "msg.h"
class msg_from_client : public msg_base
{
public:
	msg_from_client()
	{
	}
	std::string      sign_;

	int				read(boost::property_tree::ptree& jsval) 
	{
		msg_base::read(jsval);
		read_jvalue(sign_, jsval);
		return 0;
	}

	int				write(boost::property_tree::ptree& jsval)
	{
		msg_base::write(jsval);
		write_jvalue(sign_, jsval);
		return 0;
	}

  virtual ~msg_from_client() {}
};

//���е�¼�󷢵����ݰ�����Ҫ�Ӵ˼̳�
class msg_authrized : public  msg_from_client
{
public:

};

