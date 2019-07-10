#pragma once

/*
MACRO
*/
#include <iostream>
using namespace std;

#ifdef _DEBUG 
#define DEBUG_MSG(x) std::cerr << x << std::endl;
#define DEBUG_RUN(x) x;
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else 
#define DEBUG_MSG(x)
#define DEBUG_RUN(x)
#define DEBUG_NEW new
#endif