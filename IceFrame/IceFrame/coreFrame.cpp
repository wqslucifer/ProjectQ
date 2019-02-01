#include "coreFrame.h"

BaseFrame::BaseFrame() {
	empty = true;
	re_numeric_int.assign("^(\\-|\\+)?0|[1-9]\\d*$", regex::ECMAScript);
	re_numeric.assign("(^(\\-|\\+)?0|[1-9]\\d*)(\\.\\d+)?$", regex::ECMAScript);
	re_boolean_true.assign("(True|true)", regex::ECMAScript);
	re_boolean_false.assign("(False|false)", regex::ECMAScript);
}

bool BaseFrame::isEmpty()
{
	return this->empty;
}

void BaseFrame::setEmpty(bool val)
{
	this->empty = val;
}

//////////////////////////////////////////////////////////////////////////

// iceSeries
IceSeries::IceSeries() 
{
	this->_index = {};
	this->name = "";
	this->_size = 0;
	this->seriesType = -1;
	this->setEmpty(true);
}


IceSeries::IceSeries(vector<VAR> other)
{
	this->_size = other.size();
	for (auto i = 0; i < other.size(); ++i) {
		this->_index.push_back(i);
	}
	this->seriesType = getDataType(other);
	this->iceData = other;
	this->setEmpty(false);
}

IceSeries::IceSeries(IceSeries &newIce)
{
	this->_size = newIce._size;
	this->_index = newIce._index;
	this->name = newIce.name;
	this->seriesType = newIce.seriesType;
	this->iceData = newIce.iceData;
	this->setEmpty(false);
}

void IceSeries::initSeries(IceSeries *old)
{
	this->name = old->name;
	this->seriesType = old->seriesType;
	this->setEmpty(true);
}

// iloc
VAR IceSeries::iloc(int index)
{
	return this->iceData[index];
}

IceSeries IceSeries::loc(vector<bool> index)
{
	IceSeries ret;
	try
	{
		if (index.size() != this->_size) {
			throw("input length not match Series size");
		}
		ret.initSeries(this);
		for (auto i = 0; i < index.size(); ++i) {
			if (index[i])
				ret.iceData.push_back(this->iceData[i]);
		}
		ret.setEmpty(false);
		return ret;
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return ret;
	}
}

IceSeries IceSeries::loc(IceSeries &index)
{
	IceSeries ret;
	try
	{
		if (index._size != this->_size) {
			throw("input length not match Series size");
		}
		ret.initSeries(this);
		for (auto i = 0; i < index._size; ++i) {
			ret.iceData.push_back(this->iceData[i]);
		}
		ret.setEmpty(false);
		return ret;
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return ret;
	}
}

IceSeries IceSeries::subset(int start, int end) // include start, not include end
{
	if (start > end) {
		throw("subset: start larger then end");
	}
	IceSeries ret;
	ret.initSeries(this);
	for (auto i = start; i < end; ++i) {
		ret._index.push_back(this->_index[i]);
		ret.iceData.push_back(this->iceData[i]);
	}
	ret.setEmpty(false);
	return ret;
}

// operate
void IceSeries::reindex()
{
	this->_index.clear();
	for (auto i = 0; i < this->_size; ++i)
	{
		this->_index.push_back(i);
	}
}

string IceSeries::dtype(void)
{
	string ret;
	switch (this->seriesType)
	{
	case dtype_empty:
		ret = "EMPTY";
		break;
	case dtype_int:
		ret = "int";
		break;
	case dtype_double:
		ret = "double";
		break;
	case dtype_boolean:
		ret = "boolean";
		break;
	case dtype_string:
		ret = "string";
		break;

	default:
		throw("unknow data type");
		break;
	}
	return ret;
}

bool IceSeries::remove(int index)
{
	if (this->_size < 1 || index < 0 || index >= this->_size) {
		return false;
	}
	else {
		this->iceData.erase(this->iceData.begin() + index);
		this->_index.erase(this->_index.begin() + index);
		this->_size--;
		return true;
	}
}

bool IceSeries::remove(VAR val)
{
	vector<VAR>::iterator i_data = this->iceData.begin();
	vector<int>::iterator i_index = this->_index.begin();
	int count = 0;
	for (auto i = 0; i < this->_size; ++i, ++i_data, ++i_index)
	{
		if (*i_data == val)
		{
			this->iceData.erase(i_data);
			this->_index.erase(i_index);
			++count;
		}
	}
	this->_size -= count;
	if (this->_size == 0)
		this->setEmpty(true);
	return true;
}

IceSeries IceSeries::isnull(void)
{
	IceSeries ret;
	return ret;
}

int IceSeries::append(VAR other)
{
	this->iceData.push_back(other);
	this->_index.push_back(this->_size);
	++this->_size;
	return 1;
}

int IceSeries::append(vector<VAR> &other)
{
	for (auto val : other) {
		this->iceData.push_back(val);
		this->_index.push_back(this->_size);
		++this->_size;
	}
	return other.size();
}

int IceSeries::append(IceSeries &other)
{
	for (auto val : other.iceData) {
		this->iceData.push_back(val);
		this->_index.push_back(this->_size);
		++this->_size;
	}
	return other.size();
}

// utls
void IceSeries::display(void)
{
	cout << this->name << endl;
	for (auto val : this->iceData) {
		visit(coutVisitor(), val);
		cout << endl;
	}
}

bool IceSeries::clean(void)
{
	try
	{
		vector<VAR> emp;
		this->iceData.swap(emp);
		return true;
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return false;
	}
}

vector<int> IceSeries::index()
{
	return this->_index;
}

int IceSeries::size()
{
	return this->_size;
}

//////////////////////////////////////////////////////////////////////////
// iceFrame
IceFrame::IceFrame()
{
	this->rows = 0;
	this->cols = 0;
	this->getSize();
}

IceFrame::IceFrame(IceFrame &newIce)
{
	this->rows = newIce.rows;
	this->cols = newIce.cols;
	for (auto col : newIce.iceData) {
		vector<VAR> newCol=col;
		this->iceData.push_back(newCol);
	}
	this->setEmpty(newIce.isEmpty());
	this->columns = newIce.columns;
	this->colTypes = newIce.colTypes;
	this->getSize();
}

// csv
void IceFrame::loadCSV(string filename, char delim)
{
	std::fstream file;
	try
	{
		file.open(filename.c_str(), std::ios::in);
		if (!file.is_open()) {
			cout << "file not open" << endl;
			return;
		}
		rows = 0;
		cols = 0;
		// get csv file column names
		string tempLine;
		if (getline(file, tempLine)) {
			std::istringstream sin(tempLine);
			string colName;
			int colIndex = 0;
			while (getline(sin, colName, delim))
			{
				vector<VAR> oneRow;
				iceData.push_back(oneRow);
				columns.push_back(colName);
				colName_Str_Int.insert(std::pair<string, int>(colName, colIndex++));
				++cols;
			}
		}
		else {
			throw "CSV_READ_ERROR";
		}
		this->colTypes.assign(columns.size(), dtype_empty);

		// load data
		while (getline(file, tempLine)) {
			int col = 0;
			string cellBuff;
			while (getOneCell(tempLine, cellBuff, delim))
			{
				VAR cell;
				if (regex_match(cellBuff, sm, re_numeric_int) && dtype_int >= colTypes[col]) {
					colTypes[col] = dtype_int;
					cell = stoi(cellBuff);
				}
				else if (regex_match(cellBuff, sm, re_numeric) && dtype_double >= colTypes[col]) {
					colTypes[col] = dtype_double;
					cell = stod(cellBuff);
				}
				else if (cellBuff!="" && dtype_string >= colTypes[col]) {
					colTypes[col] = dtype_string;
					cell = cellBuff;
				}
				else {
					cell = monostate();
					//cell = string("NAN");
				}
				iceData[col].push_back(cell);
				++col;
			}
			for (auto c = col; c < columns.size(); c++)
				iceData[c].push_back(string(""));
			//iceData.push_back(oneRow);
			this->index.push_back(rows++);
		}
		this->setEmpty(cols + rows > 0 ? false : true);
		this->getSize();
	}
	catch (const exception& e)
	{
		cout << "unexpected error: " << e.what() << endl;
	}
}

bool IceFrame::getOneCell(string &oneLine, string &cell, char delim)
{
	/*
		read one cell from a line of csv
	*/
	size_t cellLen = 0;
	bool ifBlock = false;
	char cellEnd = delim;
	cell.clear();
	if (oneLine.size() == 0)
		return false;
	char c;
	while (cellLen < oneLine.size())
	{
		c = oneLine[cellLen];
		if (c == '\"') {
			if (!ifBlock)
				ifBlock = true;
			if (oneLine[cellLen + 1] == ',') {
				++cellLen;
				ifBlock = false;
				break;
			}
			else if (oneLine[cellLen + 1] == '\"') {
				cell += '"';
				cellLen += 2;
			}
			else {
				++cellLen;
			}
		}
		else if (c == ',' && !ifBlock) {
			break;
		}
		else{
			cell += c;
			++cellLen;
		}
	}
	if (cellLen < oneLine.size()) {
		oneLine = oneLine.substr(cellLen + 1);
	}
	else {
		oneLine = oneLine.substr(cellLen);
	}
	return true;

}

//utls
void IceFrame::getSize()
{
	this->size[0] = this->rows;
	this->size[1] = this->cols;
}

void IceFrame::display()
{
	for (int col = 0; col < cols; col++) {
		cout << this->columns[col] <<'\t';
	}
	cout << endl;
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			visit(coutVisitor(), this->iceData[col][row]);
			cout << "\t";
		}
		cout << endl;
	}
}

void IceFrame::initArrtibute(IceFrame *old, vector<int> rows, vector<int> cols)
{
	for (auto col : cols) {
		this->columns.push_back(old->columns[col]);
		this->colName_Str_Int[old->columns[col]] = old->colName_Str_Int[old->columns[col]];
	}
	for (auto row : rows) {
		this->index.push_back(old->index[row]);
	}
	this->rows = static_cast<int>(rows.size());
	this->cols = static_cast<int>(cols.size());
	this->getSize();
}

// iloc
VAR IceFrame::iloc(int row, int col)
{
	return iceData[col][row];
}

IceFrame IceFrame::iloc(vector<int> rows, vector<int> cols)
{
	IceFrame ret;
	for (auto col : cols) {
		vector<VAR> newCol;
		for (auto row : rows) {
			newCol.push_back(this->iceData[col][row]);
		}
		ret.iceData.push_back(newCol);
	}
	ret.initArrtibute(this, rows, cols);
	return ret;
}

vector<VAR> IceFrame::iloc(vector<int> rows, int col)
{
	vector<VAR> ret;
	for (auto row : rows) {
		ret.push_back(this->iceData[col][row]);
	}
	return ret;
}

vector<VAR> IceFrame::iloc(int row, vector<int> cols)
{
	vector<VAR> ret;
	for (auto col : cols) {
		ret.push_back(this->iceData[col][row]);
	}
	return ret;
}

// loc
VAR IceFrame::loc(int row, string &col)
{
	int i_col = colName_Str_Int[col];
	return iceData[i_col][row];
}

vector<VAR> IceFrame::loc(string &col)
{
	int i_col = colName_Str_Int[col];
	vector<VAR> ret = iceData[i_col];
	return ret;
}

IceFrame IceFrame::loc(vector<string> cols)
{
	IceFrame ret;
	vector<int> i_cols;
	for_each(cols.begin(), cols.end(), [&](string c) {i_cols.push_back(this->colName_Str_Int[c]); });
	for (auto col : i_cols) {
		vector<VAR> newCol;
		newCol = iceData[col];
		ret.iceData.push_back(newCol);
	}
	ret.initArrtibute(this, this->index, i_cols);
	return ret;
}

vector<VAR> IceFrame::loc(int row, vector<string> cols)
{
	vector<VAR> ret;
	vector<int> i_cols;
	for_each(cols.begin(), cols.end(), [&](string c) {i_cols.push_back(this->colName_Str_Int[c]); });
	for (auto col : i_cols)
		ret.push_back(this->iceData[col][row]);
	return ret;
}

vector<VAR> IceFrame::loc(vector<int> rows, string &col)
{
	vector<VAR> ret;
	int i_col = colName_Str_Int[col];
	for (auto row : rows)
		ret.push_back(this->iceData[i_col][row]);
	return ret;
}

IceFrame IceFrame::loc(vector<int> rows, vector<string> cols)
{
	vector<int> i_cols;
	for_each(cols.begin(), cols.end(), [&](string c) {i_cols.push_back(this->colName_Str_Int[c]); });
	IceFrame ret;
	for (auto col : i_cols) {
		vector<VAR> newCol;
		for (auto row : rows) {
			newCol.push_back(this->iceData[col][row]);
		}
		ret.iceData.push_back(newCol);
	}
	ret.initArrtibute(this, rows, i_cols);
	return ret;
}

// bool
bool IceFrame::all(int axis = 0) // axis: 0 column, 1 row
{
	try
	{
		if (axis == 0) // column
		{
			if (cols > 0) {
			}
			else {
			}
		}
		else if (axis > 0) // row
		{
		}
		else {
			throw("axis input error");
		}
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
	}
}

bool IceFrame::any(int axis=0) // axis: 0 column, 1 row
{
	try
	{
		if (axis == 0) // column
		{
		}
		else if (axis > 0) // row
		{
		}
		else {
			throw("axis input error");
		}
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
	}
}

// operate
bool IceFrame::clean()
{
	try
	{
		vector<VAR> emp;
		vector<vector<VAR> > emp_2d;
		for (auto c = iceData.begin(); c != iceData.end(); ++c)
			c->swap(emp);
		iceData.swap(emp_2d);
		return true;
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return false;
	}
}

//vector<vector<VAR>> IceFrame::concat(IceFrame &obj, int axis = 0, bool inplace = false, bool ignore_index = false);


template<typename... Args>
bool checkAll(Args... args) { return (... && args); }

template<typename... Args>
bool checkAny(Args... args) { return (... || args); }

template<typename... Ts>
void printAll(Ts&&... mXs)
{
	(cout << ... << mXs) << endl;
}

template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mXs)
{
	(mFn(mXs), ...);
}


#ifdef DEBUG
void IceFrame::testRegex(string num)
{
	cout << num << " is int:" << regex_match(num, sm, re_numeric_int) << endl;
	cout << num << " is number:" << regex_match(num, sm, re_numeric) << endl;
}

void IceSeries::testRegex(string num)
{
	cout << num << " is int:" << regex_match(num, sm, re_numeric_int) << endl;
	cout << num << " is number:" << regex_match(num, sm, re_numeric) << endl;
	cout << num << " is true:" << regex_match(num, sm, re_boolean_true) << endl;
	cout << num << " is false:" << regex_match(num, sm, re_boolean_false) << endl;
}
#endif // DEBUG


void testfunc(vector<int> input) {
	for (auto i : input)
		cout << i << endl;
}

int getDataType(vector<VAR > &data)
{
	int ret = dtype_empty;
	int rollBackIndex = 0;
	for (auto i = 0; i < data.size(); i++)
	{
		int dataType = visit(typeVisitor(), data[i]);
		if (dataType > ret) {
			ret = dataType;
			rollBackIndex = i;
		}
		if (dataType < ret) {
			if (dataType == 1)
			{
				int d = std::get<1>(data[i]);
				if (ret == 2) {
					data[i].emplace<2>(static_cast<double>(d));
				}
				else if (ret == 3) {
					data[i].emplace<3>(std::to_string(d));
				}
			}
			else if (dataType == 2)
			{
				double d = std::get<2>(data[i]);
				if (ret == 3) {
					data[i].emplace<3>(std::to_string(d));
				}
			}
		}
	}
	if (rollBackIndex > 0) {
		for (int i = 0; i < rollBackIndex; i++)
		{
			int dataType = visit(typeVisitor(), data[i]);
			if (dataType < ret) {
				if (dataType == 1)
				{
					int d = std::get<1>(data[i]);
					if (ret == 2) {
						data[i].emplace<2>(static_cast<double>(d));
					}
					else if (ret == 3) {
						data[i].emplace<3>(std::to_string(d));
					}
				}
				else if (dataType == 2)
				{
					double d = std::get<2>(data[i]);
					if (ret == 3) {
						data[i].emplace<3>(std::to_string(d));
					}
				}
			}
		}
	}
	return ret;
}


/*

int main() {
	string csvPath = "E:/project/ProjectQ/test/TitanicDataset/train.csv";
	IceFrame trainset;
	IceSeries oneSeries;
#ifdef DEBUGREGEX
	trainset.testRegex("1000");
	trainset.testRegex("0065");
	trainset.testRegex("16354.5684");
	trainset.testRegex("05");
	trainset.testRegex("0");
	trainset.testRegex("0.000");
	trainset.testRegex("+.5615");
	trainset.testRegex("+0.5615");
	trainset.testRegex("-0.5615");

	oneSeries.testRegex("true");
	oneSeries.testRegex("True");
	oneSeries.testRegex("false");
	oneSeries.testRegex("False");
	oneSeries.testRegex("tRue");
	oneSeries.testRegex("trueFalse");
	oneSeries.testRegex("falseTrue");

#endif // DEBUGREGEX
	trainset.loadCSV(csvPath);
	trainset.display();
	//subset.display();
	system("pause");
}*/