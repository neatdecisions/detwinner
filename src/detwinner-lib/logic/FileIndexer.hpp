/*
 ===============================================================================
 Name        : FileIndexer.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <regex>

#include <logic/FileSearchSettings.hpp>
#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>

namespace detwinner::logic {

//==============================================================================
// FileIndexer
//==============================================================================
class FileIndexer
{
public:
	explicit FileIndexer(const FileSearchSettings & settings);
	void performIndexing(const std::vector<std::string> & fileList,
	                     callbacks::IIndexedFileReceiver & fileReceiver,
	                     callbacks::ISearchProcessCallback::Ptr searchCallback);

private:
	enum class FileType { Unknown, Ignored, Regular, Directory };

	void processDirectory(const std::string & path,
	                      const callbacks::ISearchProcessCallback::Ptr & searchCallback,
	                      callbacks::IIndexedFileReceiver & fileReceiver);

	void processFile(const std::string & filePath,
	                 const callbacks::ISearchProcessCallback::Ptr & searchCallback,
	                 callbacks::IIndexedFileReceiver & fileReceiver);

	void processFilesInDirectory(const std::string & directoryPath,
	                             const callbacks::ISearchProcessCallback::Ptr & searchCallback,
	                             callbacks::IIndexedFileReceiver & fileReceiver);

	FileType getFileType(const std::string & filePath) const;

	const FileSearchSettings m_settings;
	std::vector<std::regex> m_includedRegexps;
};

} // namespace detwinner::logic
