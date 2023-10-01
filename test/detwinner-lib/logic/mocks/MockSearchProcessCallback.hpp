#pragma once

#include <gmock/gmock.h>
#include <logic/callbacks/ISearchProcessCallback.hpp>

namespace detwinner::logic::callbacks::mocks {

class MockSearchProcessCallback : public detwinner::logic::callbacks::ISearchProcessCallback
{
public:
	using Ptr = std::shared_ptr<MockSearchProcessCallback>;
	static Ptr Create() { return std::make_shared<MockSearchProcessCallback>(); }
	MOCK_METHOD1(onFileProcessed, void(unsigned long long size));
	MOCK_METHOD2(updateProgress, void(unsigned int progress, unsigned int total));
	MOCK_METHOD3(onDuplicateFound, void(std::size_t, unsigned long long, unsigned long long));
	MOCK_METHOD1(onStartComparing, void(unsigned int totalNumber));
	MOCK_METHOD1(onFileIndexed, void(bool skipped));
	MOCK_METHOD0(onFolderSkipped, void());
	MOCK_METHOD1(setStage, void(int stage));
	MOCK_METHOD0(pauseAndStopStatus, bool());
	MOCK_METHOD0(onInit, void());
	MOCK_METHOD0(onFinish, void());
};

} // namespace detwinner::logic::callbacks::mocks
