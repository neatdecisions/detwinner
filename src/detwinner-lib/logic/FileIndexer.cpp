/*
 ===============================================================================
 Name        : FileIndexer.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/FileIndexer.hpp>

#include <iostream>

#if __has_include(<filesystem>)
#   include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#   include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#   error <experimental/filesystem> and <filesystem> not found
#endif


namespace detwinner {
namespace logic {


//------------------------------------------------------------------------------
FileIndexer::FileIndexer(const FileSearchSettings & settings) :
	m_settings(settings)
{
	for (auto && s: m_settings.filenameRegexps)
	{
		try
		{
			m_includedRegexps.emplace_back(s);
		} catch (const std::regex_error& e)
		{
			std::cerr <<  e.what() << std::endl;
		}
	}
}


//------------------------------------------------------------------------------
void
FileIndexer::performIndexing(
	const std::vector<std::string> & fileList,
	callbacks::IIndexedFileReceiver & fileReceiver,
	callbacks::ISearchProcessCallback::Ptr_t searchCallback)
{
	for (const std::string & path: fileList)
	{
		if (searchCallback && searchCallback->pauseAndStopStatus()) return;
		processDirectory(path, searchCallback, fileReceiver);
	}
}


//------------------------------------------------------------------------------
void
FileIndexer::processDirectory(
	const std::string & path,
	const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
	callbacks::IIndexedFileReceiver & fileReceiver)
{
	try
	{
		switch (getFileType(path))
		{
			case FileType_t::kRegular:
				processFile(path, searchCallback, fileReceiver);
				break;
			case FileType_t::kDirectory:
				processFilesInDirectory(path, searchCallback, fileReceiver);
				break;
			case FileType_t::kIgnored:
				if (searchCallback) searchCallback->onFileIndexed(true);
				break;
			default:
				break;
		}
	} catch (const std::exception & ex)
	{
		std::cerr <<  "Error while processing file " << path << ": " << ex.what() << std::endl;
	} catch (...)
	{
		std::cerr <<  "Error while processing file " << path << std::endl;
	}
}


//------------------------------------------------------------------------------
void
FileIndexer::processFile(
	const std::string & filePath,
	const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
	callbacks::IIndexedFileReceiver & fileReceiver)
{
	bool isFileSkipped = true;
	try
	{
		std::error_code errorCode;
		const unsigned long long fileSize = fs::file_size(filePath, errorCode);
		if (!errorCode)
		{
			fileReceiver.receive(callbacks::IIndexedFileReceiver::FileInfo{filePath, fileSize});
			isFileSkipped = false;
		}
	} catch (...)
	{
		std::cerr <<  "Error while indexing file " << filePath << std::endl;
	}
	if (searchCallback) searchCallback->onFileIndexed(isFileSkipped);
}


//------------------------------------------------------------------------------
void
FileIndexer::processFilesInDirectory(
		const std::string & directoryPath,
		const callbacks::ISearchProcessCallback::Ptr_t & searchCallback,
		callbacks::IIndexedFileReceiver & fileReceiver)
{
	const fs::path dirPath(directoryPath);
	if (!dirPath.empty())
	{
		std::error_code errorCode;
		for (auto & it: fs::recursive_directory_iterator(dirPath, fs::directory_options::skip_permission_denied, errorCode))
		{
			if (!errorCode)
			{
				switch (getFileType(it.path().string()))
				{
					case FileType_t::kRegular:
						processFile(it.path().string(), searchCallback, fileReceiver);
						break;
					case FileType_t::kIgnored:
						if (searchCallback) searchCallback->onFileIndexed(true);
						break;
					default:
						break;
				}
			}
			if (searchCallback && searchCallback->pauseAndStopStatus()) return;
		}
	}
}


//------------------------------------------------------------------------------
FileIndexer::FileType_t
FileIndexer::getFileType(const std::string & filePath) const
{
	if (filePath.empty()) return FileType_t::kUnknown;

	if (!m_settings.searchHiddenFiles)
	{
		const fs::path fp(filePath);
		if (fp.filename().string().find(".") == 0) return FileType_t::kIgnored;
	}

	std::error_code errorCode;
	const fs::file_status fileStatus = fs::symlink_status(filePath, errorCode);
	if (errorCode) return FileType_t::kUnknown;

	if (fs::is_directory(fileStatus)) return FileType_t::kDirectory;

	if (fs::is_regular_file(fileStatus))
	{
		if (!m_settings.searchReadOnlyFiles || !m_settings.searchExecutableFiles)
		{
			const auto p = fileStatus.permissions();
			if (!m_settings.searchReadOnlyFiles)
			{
				if ( ( (p & fs::perms::group_write) == fs::perms::none ) &&
				     ( (p & fs::perms::owner_write) == fs::perms::none ) )
				{
					return FileType_t::kIgnored;
				}
			}

			if (!m_settings.searchExecutableFiles)
			{

				if ( ( (p & fs::perms::group_exec) != fs::perms::none ) &&
				     ( (p & fs::perms::owner_exec) != fs::perms::none ) )
				{
					return FileType_t::kIgnored;
				}
			}
		}

		const unsigned long long fileSize = fs::file_size(filePath, errorCode);
		if (errorCode) return FileType_t::kUnknown;
		if ( m_settings.maxFileSize && (m_settings.maxFileSize.value() <= fileSize) ) return FileType_t::kIgnored;
		if ( m_settings.minFileSize && (m_settings.minFileSize.value() >= fileSize) ) return FileType_t::kIgnored;

		bool regexMatched = m_includedRegexps.empty();
		for (auto && regex: m_includedRegexps)
		{
			if (std::regex_match(filePath, regex))
			{
				regexMatched = true;
				break;
			}
		}
		if (!regexMatched) return FileType_t::kIgnored;
		return FileType_t::kRegular;
	}
	return FileType_t::kUnknown;
}


}}
