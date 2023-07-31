
#ifndef UB_MANAGER_H
#define UB_MANAGER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

/*
	Class that load a list of upper bounds for different problems
	
	Just pass the problem name (or path with name)
	to get the upper bound of the problem
*/

class UbManager
{
	public:
		UbManager()= default;
		~UbManager()= default;

		void static Load(std::string const &filename);
		static std::map<std::string, double> bounds;

};




#endif
