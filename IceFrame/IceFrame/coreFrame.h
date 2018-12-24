#pragma once
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <variant>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/regex.hpp>

#define UNKNOWN_TYPE -1

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::map;
using std::stoi;
using std::stod;
using std::fstream;
using std::variant;
using std::istringstream;

constexpr int dtype_empty = -1;
constexpr int dtype_int = 0;
constexpr int dtype_double = 1;
constexpr int dtype_string = 2;

/*
Ice Frame:
IceFrame is a data frame designed for machine learning and other data mining algorithm
*/

class IceFrame {
public:
	int rows;
	int cols;
	int *size;
	char delim;
	vector<string> columns;
	vector<int> colTypes;
	bool isEmpty;
	boost::smatch sm;
	boost::regex numeric;
	boost::regex numeric_int;
	IceFrame() {
		delim = ',';
		isEmpty = true;
		numeric = boost::regex("^(\\-|\\+)?0|[1-9]\\d*+(\\.\\d+)?$");
		numeric_int = boost::regex("^(\\-|\\+)?0|[1-9]\\d*$");
	}
private:
	typedef variant<int, double, string> VAR;
	vector<vector<VAR>> iceData;
	map<string, int> colName_Str_Int;
	//////////////////////////////////////////////////////////////////////////
	void loadCSV(string filename);
	void getSize();
protected:
};