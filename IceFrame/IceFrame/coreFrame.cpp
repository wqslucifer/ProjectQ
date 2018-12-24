#include "coreFrame.h"

#include <iostream>
using namespace std;

void IceFrame::loadCSV(string filename)
{

}

void IceFrame::getSize()
{
	this->size = new int[2];
	this->size[0] = this->rows;
	this->size[1] = this->cols;
}

template<typename... Args>
bool all(Args... args) { return (... && args); }

template<typename... Args>
bool any(Args... args) { return (... || args); }

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


int main() {
	forArgs([](auto a) {cout << a << endl; }, 3, 4.0, "5");
	system("pause");
}