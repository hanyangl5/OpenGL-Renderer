#pragma once
// standard libs
#include <iostream>
#include <string>
// 3rd libs
// headers

namespace Log {

	inline void Err(std::string errmsg) {
		throw std::runtime_error(errmsg.c_str());
	}


	inline void Log()
	{ 
		
	}

	template <typename T, typename... Types>
	inline void Log(const T& logs, const Types &... args)
	{
		std::cout << logs;
		if (sizeof...(args) != 0)
			std::cout << " ";
		Log(args...);
	}

}

