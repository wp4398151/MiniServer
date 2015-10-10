#include "stdafx.h"

#include "test.h"
#include "UtilHelper.h"

///////////////////////////////////////////
// ����gtest

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
// ����glog
#include <glog/logging.h>
#include <windows.h>

void testgLog()
{
	//google::InitGoogleLogging(argv[0]);
	LOG(INFO) << "GLOG INFO!";
	LOG(WARNING) << "GLOG WARNING!";


	//LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies"; ����//����������ʱ�����־
	//LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";����//google::COUNTER ��¼����䱻ִ�д�������1��ʼ���ڵ�һ�����������־֮��ÿ�� 10 �������һ����־��Ϣ
	//LOG_IF_EVERY_N(INFO, (size > 1024), 10) << "Got the " << google::COUNTER << "th big cookie";����//�������ߵĽ�ϣ�����Ҫע�⣬����ÿ�� 10 ��ȥ�ж������Ƿ����㣬������������־�������ǵ�����ĳ����������£�ÿ�� 10 �����һ����־��Ϣ��
	//LOG_FIRST_N(INFO, 20) << "Got the " << google::COUNTER << "th cookie";����//�������ִ�е�ǰ 20 �ζ������־��Ȼ�������

	LOG(ERROR) << "GLOG ERROR!";
	LOG(FATAL) << "GLOG FATAL!";
	//google::ShutdownGoogleLogging();
}
