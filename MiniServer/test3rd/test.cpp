#include "stdafx.h"

#include "test.h"

///////////////////////////////////////////
// ≤‚ ‘gtest

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
// ≤‚ ‘glog
#include <glog/logging.h>
#include <windows.h>

void testgLog()
{
	LOG(INFO) << "GLOG !";
}
