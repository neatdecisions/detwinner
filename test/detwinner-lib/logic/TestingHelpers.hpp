#pragma once

#include <string>
#include <vector>

#include <logic/CommonDataTypes.hpp>

namespace detwinner::logic {

//==============================================================================
// Free functions
//==============================================================================

bool FilePathIncludesFileName(const std::string & filePath, const std::string & fileName);

bool FilePathsContainFileName(const std::vector<std::string> & filePaths, const std::string & fileName);

bool FileInfosContainFileName(const DuplicateContainer & duplicateContainer, const std::string & fileName);

} // namespace detwinner::logic
