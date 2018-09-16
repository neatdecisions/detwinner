/*
 ===============================================================================
 Name        : FileIndexer.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_FILEINDEXER_HPP_
#define LOGIC_FILEINDEXER_HPP_

#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/FileSearchSettings.hpp>
#include <regex>


namespace detwinner {
namespace logic {


//==============================================================================
// FileIndexer
//==============================================================================
class FileIndexer
{
public:
	explicit FileIndexer(const FileSearchSettings & settings);
	void performIndexing(
		const std::vector<std::string> & fileList,
		callbacks::IIndexedFileReceiver & fileReceiver,
		callbacks::ISearchProcessCallback::Ptr_t searchCallback);

private:
	enum class FileType_t
	{
		kUnknown,
		kIgnored,
		kRegular,
		kDirectory
	};

	void processDirectory(
		const std::string & path,
		const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
		callbacks::IIndexedFileReceiver & fileReceiver);

	void processFile(
		const std::string & filePath,
		const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
		callbacks::IIndexedFileReceiver & fileReceiver);

	void processFilesInDirectory(
		const std::string & directoryPath,
		const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
		callbacks::IIndexedFileReceiver & fileReceiver);

	FileType_t getFileType(const std::string & filePath) const;

	const FileSearchSettings m_settings;
	std::vector<std::regex> m_includedRegexps;
};


}}

#endif /* LOGIC_FILEINDEXER_HPP_ */
