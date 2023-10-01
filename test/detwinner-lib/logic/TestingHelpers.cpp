#include "TestingHelpers.hpp"

#include <algorithm>

namespace detwinner::logic {

//==============================================================================
// Free functions
//==============================================================================

//------------------------------------------------------------------------------
bool
FilePathIncludesFileName(const std::string & filePath, const std::string & fileName)
{
	if (filePath.size() < fileName.size()) return false;
	return filePath.find(fileName) == (filePath.size() - fileName.size());
}

//------------------------------------------------------------------------------
bool
FilePathsContainFileName(const std::vector<std::string> & filePaths, const std::string & fileName)
{
	return std::any_of(filePaths.begin(), filePaths.end(), [&fileName](const std::string & filePath) {
		return FilePathIncludesFileName(filePath, fileName);
	});
}

//------------------------------------------------------------------------------
bool
FileInfosContainFileName(const DuplicateContainer & duplicateContainer, const std::string & fileName)
{
	return std::any_of(duplicateContainer.files.begin(), duplicateContainer.files.end(),
	                   [&fileName](const DuplicateContainer::FileDataInfo & fileInfo) {
											 return FilePathIncludesFileName(fileInfo.name, fileName);
										 });
}

} // namespace detwinner::logic
