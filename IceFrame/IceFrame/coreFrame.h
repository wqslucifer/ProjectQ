#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <variant>

#define UNKNOWN_TYPE -1

//#define DEBUGREGEX

#ifdef DEBUGREGEX
#ifndef DEBUG
#define DEBUG
#endif
#endif // DEBUGREGEX

constexpr int dtype_empty = 0;
constexpr int dtype_int = 1;
constexpr int dtype_double = 2;
constexpr int dtype_boolean = 3;
constexpr int dtype_string = 4;


using std::monostate;
using std::variant;
using std::cout;
using std::endl;
using std::string;
using std::regex;
using std::smatch;
using std::vector;
using std::exception;
using std::visit;
using std::max;

using EMPTY = monostate;
using VAR = variant<EMPTY, int, double, bool, string>;


/*
Ice Frame:
IceFrame is a data frame designed for machine learning and other data mining algorithm
*/

struct coutVisitor {
	void operator() (const EMPTY input) const {
		cout<<"NAN";
	}
	void operator() (const int input) const {
		cout << input;
	}
	void operator() (const double input) const {
		cout << input;
	}
	void operator() (const bool input) const {
		cout << input;
	}
	void operator() (const string input) const {
		cout << input;
	}
};

struct cmpVisitor {
	bool operator() (const int input1, const int input2) const {
		return input1 == input2;
	};
	bool operator() (const double input1, const double input2) const {
		return input1 == input2;
	};
	bool operator() (const bool input1, const bool input2) const {
		return input1 == input2;
	};
	bool operator() (const string input1, const string input2) const {
		return input1 == input2;
	};
};

struct typeVisitor {
	int operator()(const EMPTY input) const {
		return dtype_empty;
	};
	int operator()(const int input) const {
		return dtype_int;
	};
	int operator()(const double input) const {
		return dtype_double;
	};
	int operator()(const bool input) const {
		return dtype_boolean;
	};
	int operator()(const string input) const {
		return dtype_string;
	};
};

int getDataType(vector<variant<EMPTY, int, double, bool, string> > &data)
{
	int ret = dtype_empty;
	for (auto d : data)
	{
		ret = max<int>(ret, visit(typeVisitor(), d));
	}
	return ret;
}

class BaseFrame{
public:
	BaseFrame();
	virtual ~BaseFrame() {}
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
	bool empty;
protected:
	void setEmpty(bool val);
};

BaseFrame::BaseFrame() {
	empty = true;
	re_numeric_int.assign("^(\\-|\\+)?0|[1-9]\\d*$", regex::ECMAScript);
	re_numeric.assign("(^(\\-|\\+)?0|[1-9]\\d*)(\\.\\d+)?$", regex::ECMAScript);
	re_boolean_true.assign("(True|true)", regex::ECMAScript);
	re_boolean_false.assign("(False|false)", regex::ECMAScript);
}

//////////////////////////////////////////////////////////////////////////
class IceSeries:public BaseFrame {
public:
	vector<int> index;
	string name;

	IceSeries();
	IceSeries(vector<VAR> &other);
	IceSeries(IceSeries &newIce);
	~IceSeries() { this->clean(); };
	// iloc
	VAR iloc(int index);
	IceSeries* loc(vector<bool> index);
	IceSeries* loc(IceSeries &index);
	IceSeries subset(int start, int end);
	// operate
	string dtype(void);
	bool remove(int index);
	bool remove(VAR val);
	void reindex(void);
	IceSeries isnull(void);
	int append(VAR other);
	int append(vector<VAR> &other);
	int append(IceSeries &other);
	// calculate
	double sum(IceSeries &out, int axis = 0, bool numeric_only= true, bool skipNA=true); // axis 0--col 1--row
	double mean(IceSeries &out, int axis = 0, bool numeric_only = true, bool skipNA = true);
	double median(IceSeries &out, int axis = 0, bool numeric_only = true, bool skipNA = true);
	double max(IceSeries &out, int axis = 0, bool numeric_only = true, bool skipNA = true);
	double min(IceSeries &out, int axis = 0, bool numeric_only = true, bool skipNA = true);
	// utls
	vector<VAR> data();
	void display(void);
	bool clean(void);
	bool all(void);
	bool any(void);
	void initSeries(IceSeries *old);
#ifdef DEBUG
	void testRegex(string num);
#endif // DEBUG
private:
	size_t size;
	vector<VAR> iceData;
	int seriesType;
protected:

};

//////////////////////////////////////////////////////////////////////////

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
	void saveCSV(vector<vector<VAR> > obj, string filename);
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
	// loc: bool
	vector<VAR> loc(int row, vector<bool> cols);
	vector<VAR> loc(vector<bool> rows, int col);
	vector<VAR> loc(vector<bool> rows, string col);
	IceFrame loc(vector<bool> rows, vector<string> cols);
	IceFrame loc(vector<int> rows, vector<bool> cols);
	IceFrame loc(vector<bool> rows, vector<bool> cols);


	// bool
	bool all(int axis);
	bool any(int axis);
	bool isnull();
	// operate
	vector<vector<VAR> > concat(IceFrame &obj, int axis=0, bool inplace=false, bool ignore_index=false);
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
	vector<vector<VAR> > iceData;
	std::map<string, int> colName_Str_Int;
	//////////////////////////////////////////////////////////////////////////
	void initArrtibute(IceFrame *old, vector<int> rows, vector<int> cols);
	void getSize();
	//bool getOneCell(string &oneLine, string &cell, char delim);
protected:
};

