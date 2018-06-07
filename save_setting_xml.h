#pragma once
//使用xml保存设置是因为xml考虑各平台兼容性问题
#include <string>
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"

using namespace boost::property_tree;
class		xml_setting
{
public:
	explicit xml_setting(std::string fname)
	{
		fname_ = fname;
		try	{
			std::ifstream ifstm(fname);
			boost::property_tree::xml_parser::read_xml(ifstm, setting_);
		}
		catch (std::runtime_error& e){
			int		a = 0;
		}
	}
	template<class val_t>
	void		set(std::string path, val_t v)
	{
		setting_.put(path, v);
	}

	template<class val_t>
	val_t		get(std::string path, val_t def)
	{
		if(setting_.find(path) == setting_.not_found()){
			return def;
		}
		else
			return setting_.get<val_t>(path);
	}
	
	ptree&	get_child(std::string path)
	{
		if (setting_.find(path) == setting_.not_found()){
			setting_.put_child(path, ptree());
			return setting_.get_child(path);
		}
		else
			return setting_.get_child(path);
	}

	void		flush()
	{
		std::ofstream ofstm(fname_, std::ios::ate);
		xml_parser::write_xml(ofstm, setting_);
	}

protected:
	ptree setting_;
	std::string		fname_;
};
