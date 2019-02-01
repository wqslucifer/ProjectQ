#include "pch.h"
#include "../IceFrame/coreFrame.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/IceFrame.lib")
#else
#pragma comment(lib, "../x64/Release/IceFrame.lib")
#endif

// /D GTEST_HAS_TR1_TUPLE=0
// /D GTEST_USE_OWN_TR1_TUPLE = 1

class IceSeriesTest : public ::testing::Test
{
protected:
	void SetUp() override{
		var_test1 = new vector<VAR>();
		var_test2 = new vector<VAR>({ "asdf","","te","234","sdafg","" });
		s0 = new IceSeries({ 1,2,3,4,5 });
		s1 = new IceSeries({ 2,5,3,7,0,2.3,4.2,0,0,2,3,3,10 });
		s2 = new IceSeries({ "asdf","","te","234","sdafg","" });
		s3 = new IceSeries({ "test",123,"t","345" });
		s4 = new IceSeries({ "false","True","true","False","False" });
		s5 = new IceSeries({ true, false, true, true, false });
	}
	void TearDown() override{
		delete s0;
		delete s1;
		delete s2;
		delete s3;
		delete s4;
		delete s5;
		delete var_test1;
		delete var_test2;
	}
	IceSeries *s0;
	IceSeries *s1;
	IceSeries *s2;
	IceSeries *s3;
	IceSeries *s4;
	IceSeries *s5;
	vector<VAR> *var_test1;
	vector<VAR> *var_test2;
};

TEST_F(IceSeriesTest, vectorInputTest)
{
	EXPECT_TRUE(var_test1->empty());
	string test2[6] = { "asdf","","te","234","sdafg","" };
	for (int i = 0; i < 6; i++) {
		string tmp = std::get<string>(var_test2->at(i));
		EXPECT_STREQ(test2[i].c_str(), tmp.c_str());
	}
}

TEST_F(IceSeriesTest, constructTest)
{
	// test size function and construction function
	EXPECT_EQ(5, s0->size());
	EXPECT_EQ(13, s1->size());
	EXPECT_EQ(6, s2->size());
	EXPECT_EQ(4, s3->size());
	EXPECT_EQ(5, s4->size());
	EXPECT_EQ(5, s5->size());
}

TEST_F(IceSeriesTest, dtypeTest)
{
	EXPECT_STREQ("int", s0->dtype().c_str());
	EXPECT_STREQ("double", s1->dtype().c_str());
	EXPECT_STREQ("string", s2->dtype().c_str());
	EXPECT_STREQ("string", s3->dtype().c_str());
	EXPECT_STREQ("string", s4->dtype().c_str());
	EXPECT_STREQ("int", s5->dtype().c_str());
}

TEST_F(IceSeriesTest, ilocTest) 
{
	vector<int> testCase0 = { 1,2,3,4,5 };
	vector<double> testCase1 = { 2,5,3,7,0,2.3,4.2,0,0,2,3,3,10 };
	vector<string> testCase3 = { "test","123","t","345" };
	for (auto i = 0; i < s0->size(); i++) {
		EXPECT_EQ(testCase0[i], std::get<int>(s0->iloc(i)));
	}
	for (auto i = 0; i < s1->size(); i++) {
		EXPECT_EQ(testCase1[i], std::get<double>(s1->iloc(i)));
	}
	for (auto i = 0; i < s3->size(); i++) {
		EXPECT_EQ(testCase3[i], std::get<string>(s3->iloc(i)));
	}
}


int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}