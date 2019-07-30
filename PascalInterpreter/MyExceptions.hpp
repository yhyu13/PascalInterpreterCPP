#pragma once
#include <exception>
#include <string>
#include "MyDebug.hpp"

namespace MyExceptions
{
	class MsgExecption : std::exception
	{
	private:
		std::string m_msg;

	public:
		explicit MsgExecption(const std::string& msg)
			:
			m_msg(msg)
		{}

		explicit MsgExecption(const std::string& msg, MyDebug::SrouceFileDebugger* sfd, unsigned int pos)
		{
			if (sfd)
				m_msg = sfd->GetDebugString(pos) + msg;
			else
				m_msg = msg;
		}

		virtual const char* what() const override
		{
			return m_msg.c_str();
		}
	};
}
