#include "msg.h"
#include "utility.h"

template<> 
void	stream_base::read_data<char>(char* v, unsigned int element_count, const char*&data_s, bool swap_order)
{
	if (rd_fail_) return;
	memset(v, 0, element_count);

	int buff_left = rd_buff_len_ - (data_s - rd_begin_);
	if (buff_left <= 0)	{
		rd_fail_ = true;
		return;
	}

	int len = 0;
	unsigned char len1 = *(unsigned char*) data_s;
	data_s++;
	if (len1 == 0xFF){
		len = int((unsigned char)(*data_s) << 8); data_s++;
		len |= (unsigned char)(*data_s);
		data_s++;
	}
	else{
		len = len1;
	}

	buff_left = rd_buff_len_ - (data_s - rd_begin_);

	if (buff_left < len){
		rd_fail_ = true;
		return;
	}

	if(len == 0) return;

	memcpy(v, data_s, len);

	//添加终结符.以防攻击
	if(len < element_count){
		v[len] = 0;
		data_s += len + 1;
	}
	else {
		v[element_count - 1] = 0;
		data_s += element_count;
	}
}


template<>
void stream_base::write_data(char* v, unsigned int element_count, char*& data_s, bool swap_order)
{
	if (wt_fail_) return;
	char* vv = v;
	//找出字符串长度
	int len = 0;
	for (;len < element_count; len++, vv++)
	{
		if(*vv == 0){	break; }
	}

	int buff_left = wt_buff_len_ - (data_s - wt_begin_);
	if (buff_left <= 0){
		wt_fail_ = true;
		return;
	}

	if (len > 0xFF){
		*(unsigned char*)data_s = 0xFF; data_s++;
		*data_s = ((len >> 8) & 0xFF); data_s++;
	}
	*(unsigned char*)data_s = (len & 0xFF); data_s++;

	if(len == 0) return;		//没内容

	buff_left = wt_buff_len_ - (data_s - wt_begin_);
	if (buff_left < len){
		wt_fail_ = true;
		return;
	}

	memcpy(data_s, v, len);
	//整个长度内未包含终结符，则强制最后一个字节为终结符
	if(len < element_count){ 
		data_s[len] = 0;
		data_s += len + 1;
	}
	//添加一个终结符
	else{
		data_s[element_count - 1] = 0;
		data_s += element_count;
	}
}

stream_buffer msg_json_form::alloc_io_buffer()
{
	boost::shared_array<char> buff(new char[content_.size() + 8]);
	return stream_buffer(buff, 0, content_.size() + 8); //8为content前面的数据长度.
}

void msg_json_form::set_content(std::string json_dat)
{
	//如果数据长度大于1024,启用压缩
	if (json_dat.length() > 0x400){
		is_compressed_ = 1;
		content_ = gzip_gz(json_dat);
	}
	else {
		content_ = json_dat;
		is_compressed_ = 0;
	}
}

int msg_json_form::read(const char*& data_s, unsigned int l)
{
	msg_base::read(data_s, l);
	read_data(sub_cmd_, data_s);
	read_data(is_compressed_, data_s);

	int len = head_.len_ - 8;	//8为content前面的数据长度.
	boost::shared_array<unsigned char> buff(new unsigned char[len]);
	read_data<unsigned char>(buff.get(), len, data_s);

	content_.insert(content_.end(), (char*)buff.get(), (char*)buff.get() + len);
	//如果压缩过,则解压
	if (is_compressed_){
		content_ = ungzip_gz(content_);
	}
	return 0;
}

int msg_json_form::write(char*& data_s, unsigned int l)
{
	msg_base::write(data_s, l);
	write_data(sub_cmd_, data_s);
	write_data(is_compressed_, data_s);
	write_data<unsigned char>((unsigned char*)content_.c_str(), content_.length(),  data_s);
	return 0;
}
