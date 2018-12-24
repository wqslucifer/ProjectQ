#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>

using std::string;

/*
Ice Frame:
IceFrame is a data frame designed for machine learning and other data mining algorithm
*/

class IceFrame {
public:
	int rows;
	int cols;
	int *size;
private:
	void loadCSV(string filename);
	void getSize();
protected:
};

