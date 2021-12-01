#ifndef SPLIT_H
#define SPLIT_H

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
//Realiza split de strings

namespace stringHelper
{
	const std::vector<std::string> explode(const std::string& s, const char& c);
}
#endif