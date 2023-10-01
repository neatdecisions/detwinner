/*
 ============================================================================
 Name        : DuplicateImageFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#pragma once
#include <string>
#include <vector>

#include <logic/CommonDataTypes.hpp>
#include <logic/FileSearchSettings.hpp>
#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>

namespace detwinner::logic {

//==============================================================================
// DuplicateImageFinder
//==============================================================================
class DuplicateImageFinder
{
public:
	DuplicatesList find(const std::vector<std::string> & folderList,
	                    const FileSearchSettings & searchSettings,
	                    const callbacks::ISearchProcessCallback::Ptr & searchProcessCallback);

private:
	struct FileToProcessReceiver : callbacks::IIndexedFileReceiver
	{
		void receive(FileInfo && fileInfo) override;
		std::vector<std::string> fileNames;
	};

	static constexpr unsigned short kDefaultSensitivity = 85;
	static constexpr bool kDefaultProcessRotations = false;
};

} // namespace detwinner::logic
