#pragma once
#include <sstream>
#include <type_traits>
#include <typeinfo> 

template<class T> struct is_shared_ptr : std::false_type {};
template<class T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
inline int check_is_shared_ptr(const T& t) {
	return is_shared_ptr<T>::value;
}


namespace MyTemplates
{
	template <typename T>
	std::string Str(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}
