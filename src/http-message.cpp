#include "http-message.h"
#include<sstream>

namespace http {

	HttpMessage::HttpMessage(std::string_view raw) {
		if (raw != "") parse(raw);
	}

	HttpMessage::~HttpMessage() {

	}

	bool HttpMessage::parse(std::string_view raw) {
		auto startline_end	= raw.find("\r\n");
		auto header_offset	= startline_end + 2;
		auto header_end		= raw.find("\r\n\r\n");
		auto body_offset	=  header_end + 4;

		mStartline = raw.substr(0, startline_end);
		{
			std::string temp(mStartline.data() + mStartline.find(' ') + 1);
			mTarget = temp.substr(0, temp.find(' '));
			mMethod = mStartline.substr(0, mStartline.find(' '));
		}

		auto raw_header = raw.substr(
			header_offset, header_end - header_offset
		);

		while (raw_header.find("\r\n") != std::string::npos) {
			auto header_del_iter = raw_header.find("\r\n");
			auto kv_del_iter = raw_header.find(':');
			auto value_len = header_del_iter - kv_del_iter - 2;

			auto header_data = raw_header.substr(0, header_del_iter);
			auto key = raw_header.substr(0, kv_del_iter);
			auto value = raw_header.substr(kv_del_iter + 2, value_len);

			mHeader[std::string(key)] = std::string(value);
			raw_header = raw_header.substr(header_del_iter + 2);
		}

		mBody = raw.substr(body_offset);

		return true;
	}

	std::string HttpMessage::str() {
		std::stringstream ss;
		ss << mStartline << "\r\n";
		for (const auto& prop : mHeader) {
			ss << prop.first << ": " << prop.second << "\r\n";
		}
		ss << "\r\n\r\n" << mBody;
		return ss.str();
	}

	std::string HttpMessage::getHeader(std::string_view key) {
		if (auto iter = mHeader.find(key.data()); iter != std::end(mHeader)) {
			return iter->second;
		}
		return "";
	}

	void HttpMessage::setHeader(std::string_view key, std::string_view value) {
		mHeader[key.data()] = value.data();
	}

	void HttpMessage::setBody(std::string_view body) {
		mBody = body;
	}


}
