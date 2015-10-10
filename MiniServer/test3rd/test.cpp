#include "stdafx.h"

#include "test.h"
#include "UtilHelper.h"

///////////////////////////////////////////
// 测试gtest

#include "gtest/gtest.h"

TEST(SampleTest, One) {
	EXPECT_EQ(1, 1);
}

void testgTest(int* argc, _TCHAR** argv)
{
	testing::InitGoogleTest(argc, argv);
	RUN_ALL_TESTS();
}

///////////////////////////////////////////
// 测试glog
#include <glog/logging.h>
#include <windows.h>

void testgLog()
{
	//google::InitGoogleLogging(argv[0]);
	LOG(INFO) << "GLOG INFO!";
	LOG(WARNING) << "GLOG WARNING!";


	//LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies"; 　　//当条件满足时输出日志
	//LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";　　//google::COUNTER 记录该语句被执行次数，从1开始，在第一次运行输出日志之后，每隔 10 次再输出一次日志信息
	//LOG_IF_EVERY_N(INFO, (size > 1024), 10) << "Got the " << google::COUNTER << "th big cookie";　　//上述两者的结合，不过要注意，是先每隔 10 次去判断条件是否满足，如果滞则输出日志；而不是当满足某条件的情况下，每隔 10 次输出一次日志信息。
	//LOG_FIRST_N(INFO, 20) << "Got the " << google::COUNTER << "th cookie";　　//当此语句执行的前 20 次都输出日志，然后不再输出

	LOG(ERROR) << "GLOG ERROR!";
	LOG(FATAL) << "GLOG FATAL!";
	//google::ShutdownGoogleLogging();
}
