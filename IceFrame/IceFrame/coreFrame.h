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
#include <regex>

#define UNKNOWN_TYPE -1

// #define DEBUGREGEX

#ifdef DEBUGREGEX
#ifndef DEBUG
#define DEBUG
#endif
#endif // DEBUGREGEX

using namespace std;

constexpr int dtype_empty = -1;
constexpr int dtype_int = 0;
constexpr int dtype_double = 1;
constexpr int dtype_string = 2;

/*
Ice Frame:
IceFrame is a data frame designed for machine learning and other data mining algorithm
*/

struct coutVisitor {
	void operator()(const int input) const {
		cout << input;
	}
	void operator()(const double input) const {
		cout << input;
	}
	void operator()(const string& input) const {
		cout << input;
	}
};

class IceFrame {
public:
	int rows;
	int cols;
	int *size;
	vector<string> columns;
	vector<int> colTypes;
	bool isEmpty;
	smatch sm;
	regex numeric;
	regex numeric_int;
	IceFrame() {
		delim = ',';
		isEmpty = true;
		numeric_int.assign("^(\\-|\\+)?0|[1-9]\\d*$", std::regex::ECMAScript);
		numeric.assign("(^(\\-|\\+)?0|[1-9]\\d*)(\\.\\d+)?$", std::regex::ECMAScript);
	}
	void setDelim(char delim);
	void loadCSV(string filename);
	void display(void);
	auto iloc(int row, int col);
	void loc(variant<int,string> row, variant<int, string> col);
	bool getOneCell(string &oneLine, string &cell, char delim);
#ifdef DEBUG
	void testRegex(string num);
#endif // DEBUG
private:
	char delim;
	typedef variant<int, double, string> VAR;
	vector<vector<VAR>> iceData;
	map<string, int> colName_Str_Int;
	//////////////////////////////////////////////////////////////////////////
	void getSize();
	//bool getOneCell(string &oneLine, string &cell, char delim);
protected:
};