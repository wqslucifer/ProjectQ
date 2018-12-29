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

//#define DEBUGREGEX

#ifdef DEBUGREGEX
#ifndef DEBUG
#define DEBUG
#endif
#endif // DEBUGREGEX

using namespace std;

constexpr int dtype_empty = -1;
constexpr int dtype_int = 0;
constexpr int dtype_double = 1;
constexpr int dtype_boolean = 2;
constexpr int dtype_string = 3;

typedef variant<int, double, bool, string> VAR;

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
	void operator()(const bool input) const {
		cout << input;
	}
	void operator()(const string& input) const {
		cout << input;
	}
};



class BaseFrame{
public:
	BaseFrame();
	virtual ~BaseFrame() {}
	bool empty;
	smatch sm;
	regex re_numeric;
	regex re_numeric_int;
	regex re_boolean_true;
	regex re_boolean_false;
	vector<int> index;
	string NA_string;
	bool isEmpty();
	virtual void display(void) = 0;
	virtual bool clean(void) = 0;
private:
};

BaseFrame::BaseFrame() {
	empty = true;
	re_numeric_int.assign("^(\\-|\\+)?0|[1-9]\\d*$", std::regex::ECMAScript);
	re_numeric.assign("(^(\\-|\\+)?0|[1-9]\\d*)(\\.\\d+)?$", std::regex::ECMAScript);
	re_boolean_true.assign("(True|true)", std::regex::ECMAScript);
	re_boolean_false.assign("(False|false)", std::regex::ECMAScript);
}


class IceSeries:public BaseFrame {
public:
	int size;
	vector<int> index;
	string name;
	int seriesType;
	IceSeries();
	IceSeries(IceSeries &newIce);
	~IceSeries() { this->clean(); };
	// iloc
	VAR iloc(int index);
	IceSeries subset(int start, int end);
	// operate
	string dtype(void);
	bool remove(int index);
	bool remove(VAR val);
	void reindex();
	// utls
	void display(void);
	bool clean(void);
	bool all(void);
	bool any(void);
	void initSeries(IceSeries *old);
#ifdef DEBUG
	void testRegex(string num);
#endif // DEBUG
private:
	vector<VAR> iceData;
protected:
};


class IceFrame:public BaseFrame {
public:
	int rows;
	int cols;
	int size[2] = { 0,0 };
	vector<string> columns;
	vector<int> colTypes;
	vector<int> index;
	IceFrame();
	IceFrame(IceFrame &newIce);
	~IceFrame() { clean(); };
	// csv
	void loadCSV(string filename, char delim = ',');
	bool getOneCell(string &oneLine, string &cell, char delim = ',');
	void saveCSV(vector<vector<VAR>> obj, string filename);
	// iloc
	VAR iloc(int row, int col);
	IceFrame iloc(vector<int> rows, vector<int> cols);
	vector<VAR> iloc(vector<int> rows, int col);
	vector<VAR> iloc(int row, vector<int> cols);
	// loc: locate columns
	VAR loc(int row, string &col);
	vector<VAR> loc(string &col);
	IceFrame loc(vector<string> cols);
	vector<VAR> loc(int row, vector<string> cols);
	vector<VAR> loc(vector<int> rows, string &col);
	IceFrame loc(vector<int> rows, vector<string> cols);
	// bool
	bool all(int axis);
	bool any(int axis);
	bool isnull();
	// operate
	vector<vector<VAR>> concat(IceFrame &obj, int axis=0, bool inplace=false, bool ignore_index=false);
	//vector<vector<VAR>> merge(vector<vector<VAR>> obj, vector<int> left_on, vector<int> right_on);

	// delete
	bool delColumn(int col);
	bool delColumn(string col);
	bool delColumn(vector<int> cols);
	bool delColumn(vector<string> cols);
	bool delRow(int row);
	bool delRow(vector<int> rows);
	bool remove(VAR val);
	bool remove(int row, int col);
	bool remove(int row, string col);
	vector<string> dtype(void);
	// utls
	void display(void);
	bool clean(void);
#ifdef DEBUG
	void testRegex(string num);
#endif // DEBUG
private:
	vector<vector<VAR>> iceData;
	map<string, int> colName_Str_Int;
	//////////////////////////////////////////////////////////////////////////
	void initArrtibute(IceFrame *old, vector<int> rows, vector<int> cols);
	void getSize();
	//bool getOneCell(string &oneLine, string &cell, char delim);
protected:
};