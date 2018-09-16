#ifndef TEST_LOGIC_MOCKS_MOCKIMAGEFINDERCALLBACK_HPP_
#define TEST_LOGIC_MOCKS_MOCKIMAGEFINDERCALLBACK_HPP_

#include <gmock/gmock.h>
#include <logic/callbacks/IImageFinderCallback.hpp>


namespace detwinner {
namespace logic {
namespace callbacks {
namespace mocks {


class MockImageFinderCallback : public detwinner::logic::callbacks::IImageFinderCallback
{
public:
	using Ptr = std::shared_ptr<MockImageFinderCallback>;
	static Ptr Create() { return std::make_shared<MockImageFinderCallback>(); }
	MOCK_METHOD2(imgIndexingProgress, void(int, int));
	MOCK_METHOD2(imgComparingProgress, void(int, int));
	MOCK_METHOD2(imgOrganizingProgress, void(int, int));
	MOCK_METHOD3(similarImagesFound, void(std::size_t, unsigned long long, unsigned long long));
	MOCK_METHOD0(pauseAndStopStatus, bool());
};


}}}}

#endif
