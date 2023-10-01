/*
 ===============================================================================
 Name        : DuplicateFilesFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include <logic/CommonDataTypes.hpp>
#include <logic/FileSearchSettings.hpp>
#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>

namespace detwinner::logic {

//==============================================================================
// DuplicateFilesFinder
//==============================================================================
class DuplicateFilesFinder
{
public:
	DuplicatesList find(const std::vector<std::string> & folderList,
	                    const FileSearchSettings & searchSettings,
	                    const callbacks::ISearchProcessCallback::Ptr & searchProcessCallback);

private:
	using FileSizeMapping = std::map<unsigned long long, std::vector<std::string>>;

	struct FileMappingReceiver : callbacks::IIndexedFileReceiver
	{
		void receive(FileInfo && fileInfo) override;
		FileSizeMapping mapping;
	};

	DuplicatesList calculateHashes(FileSizeMapping & totalMap,
	                               callbacks::ISearchProcessCallback::Ptr searchProcessCallback) const;
};

} // namespace detwinner::logic
