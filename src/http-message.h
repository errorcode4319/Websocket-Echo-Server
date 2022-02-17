#ifndef __HTTP_MESSAGE_H
#define __HTTP_MESSAGE_H

#include <iostream>
#include <unordered_map>

namespace http {

	class HttpMessage {

	public:
		HttpMessage(std::string_view raw = "");
		~HttpMessage();

		bool parse(std::string_view raw);

		std::string str();

		std::string getTarget() const { return mTarget; };
		std::string getMethod() const { return mMethod; };
		std::string getHeader(std::string_view key);
		std::string getBody() const { return mBody; };

		void setHeader(std::string_view key, std::string_view value);
		void setBody(std::string_view body);


	private:
		std::string mTarget;
		std::string mMethod;
		std::string mStartline;
		std::unordered_map<std::string, std::string> mHeader;
		std::string mBody;
	};

}

#endif // __HTTP_MESSAGE_H