#ifndef MISC_COMPAT_OPTIONAL_HPP_
#define MISC_COMPAT_OPTIONAL_HPP_


#if __has_include(<optional>)

#include <optional>
namespace stdx {
	using namespace ::std;
}
#elif __has_include(<experimental/optional>)
#include <experimental/optional>
namespace stdx {
	using namespace ::std;
	using namespace ::std::experimental;
}

#else
#	error <experimental/optional> and <optional> not found
#endif


#endif
