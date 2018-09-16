#ifndef TEST_LOGIC_TESTINGHELPERS_HPP_
#define TEST_LOGIC_TESTINGHELPERS_HPP_

#include <string>
#include <logic/CommonDataTypes.hpp>


namespace detwinner {
namespace logic {


//==============================================================================
// Free functions
//==============================================================================

bool FilePathIncludesFileName(
	const std::string & filePath,
	const std::string & fileName);

bool FilePathsContainFileName(
	const std::vector<std::string> & filePaths,
	const std::string & fileName);

bool FileInfosContainFileName(
	const DuplicateContainer & duplicateContainer,
	const std::string & fileName);

}}

#endif
