#include "coreFrame.h"


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
	this->size = 0;
	this->seriesType = -1;
	this->setEmpty(true);
}

IceSeries::IceSeries(vector<VAR> &other)
{
	this->size = other.size();
	for (auto i = 0; i < other.size(); ++i) {
		this->index.push_back(i);
	}
	this->seriesType = getDataType(other);
	this->iceData = other;
	this->setEmpty(false);
}

IceSeries::IceSeries(IceSeries &newIce)
{
	this->size = newIce.size;
	this->index = newIce.index;
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
	try
	{
		if (index.size() != this->size) {
			throw("input length not match Series size");
		}
		IceSeries ret;
		ret.initSeries(this);
		for (auto i = 0; i < index.size(); ++i) {
			if (index[i])
				ret.iceData.push_back(this->iceData[i]);
		}
		ret.setEmpty(false);
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

IceSeries IceSeries::loc(IceSeries &index)
{
	try
	{
		if (index.size != this->size) {
			throw("input length not match Series size");
		}
		IceSeries ret;
		ret.initSeries(this);
		for (auto i = 0; i < index.size; ++i) {
			ret.iceData.push_back(this->iceData[i]);
		}
		ret.setEmpty(false);
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
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
		ret.index.push_back(this->index[i]);
		ret.iceData.push_back(this->iceData[i]);
	}
	ret.setEmpty(false);
	return ret;
}

// operate
void IceSeries::reindex()
{
	this->index.clear();
	for (auto i = 0; i < this->size; ++i)
	{
		this->index.push_back(i);
	}
}

string IceSeries::dtype(void)
{
	string ret;
	switch (this->seriesType)
	{
	case dtype_empty:
		ret = "NAN";
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
	if (this->size < 1 || index < 0 || index >= this->size) {
		return false;
	}
	else {
		this->iceData.erase(this->iceData.begin() + index);
		this->index.erase(this->index.begin() + index);
		this->size--;
		return true;
	}
}

bool IceSeries::remove(VAR val)
{
	vector<VAR>::iterator i_data = this->iceData.begin();
	vector<int>::iterator i_index = this->index.begin();
	int count = 0;
	for (auto i = 0; i < this->size; ++i, ++i_data, ++i_index)
	{
		if (*i_data == val)
		{
			this->iceData.erase(i_data);
			this->index.erase(i_index);
			++count;
		}
	}
	this->size -= count;
	if (this->size == 0)
		this->setEmpty(true);
	return true;
}

IceSeries IceSeries::isnull(void)
{
	IceSeries ret;
	return ret;
}

// utls
void IceSeries::display(void)
{
	cout << this->name << endl;
	for (auto val : this->iceData) {
		std::visit(coutVisitor(), val);
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
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
		return false;
	}
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
	fstream file;
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
			istringstream sin(tempLine);
			string colName;
			int colIndex = 0;
			while (getline(sin, colName, delim))
			{
				vector<VAR> oneRow;
				iceData.push_back(oneRow);
				columns.push_back(colName);
				colName_Str_Int.insert(pair<string, int>(colName, colIndex++));
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
					cell = std::monostate();
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
	catch (const std::exception& e)
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
			std::visit(coutVisitor(), this->iceData[col][row]);
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
	catch (const std::exception& e)
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
	catch (const std::exception& e)
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
		vector<vector<VAR>> emp_2d;
		for (auto c = iceData.begin(); c != iceData.end(); ++c)
			c->swap(emp);
		iceData.swap(emp_2d);
		return true;
	}
	catch (const std::exception& e)
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
}