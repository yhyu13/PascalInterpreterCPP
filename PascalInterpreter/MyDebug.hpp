#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "MyTemplates.hpp"
#include "MyExceptions.hpp"
using namespace std;

#ifdef _DEBUG 
#define DEBUG_MSG(x) std::cerr << x << std::endl;
#define DEBUG_RUN(x) x;
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else 
#define DEBUG_MSG(x)
#define DEBUG_RUN(x)
#endif

// TODO: Unit Testing
class SrouceFileDebugger
{
public:
	SrouceFileDebugger() {};
	explicit SrouceFileDebugger(std::string filename, std::string oneliner, std::vector<std::string> vec)
		:
		m_filename(filename),
		m_oneliner(oneliner),
		m_vec(vec)
	{
	}

	void DebugPrint(unsigned int pos)
	{

		if (pos > (m_oneliner.size() - 1))
		{
			throw MyExceptions::InterpreterExecption("DebugError(SrouceFileDebugger): position " + MyTemplates::Str(pos) \
				+ " out of size of " + MyTemplates::Str(m_oneliner.size()) + ".");
		}

		unsigned int line = 0;
		unsigned int col = 0;

		for (unsigned int i = 0; i < pos; i++)
		{
			col++;
			if (m_oneliner[i] == '\n')
			{
				line++;
				col = 0;
			}
		}
		DEBUG_MSG("File---> '" + m_filename + "' at line " + MyTemplates::Str(line) + " col " + MyTemplates::Str(col));
		DEBUG_MSG(std::string(8,' ') + m_vec[line]);
		DEBUG_MSG(std::string(8 + col, ' ') + "^");
	}

private:
	std::string m_filename;
	std::string m_oneliner;
	std::vector<std::string> m_vec;
};

