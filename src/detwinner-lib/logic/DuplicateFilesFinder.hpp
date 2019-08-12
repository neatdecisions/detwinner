/*
 ===============================================================================
 Name        : DuplicateFilesFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_DUPLICATEFILESFINDER_HPP_
#define LOGIC_DUPLICATEFILESFINDER_HPP_

#include <map>
#include <string>
#include <vector>

#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/CommonDataTypes.hpp>
#include <logic/FileSearchSettings.hpp>


namespace detwinner {
namespace logic {


//==============================================================================
// DuplicateFilesFinder
//==============================================================================
class DuplicateFilesFinder
{
public:
	DuplicatesList_t find(
			const std::vector<std::string> & folderList,
			const FileSearchSettings & searchSettings,
			const callbacks::ISearchProcessCallback::Ptr_t & searchProcessCallback);

private:
	using FileSizeMapping_t = std::map< unsigned long long, std::vector<std::string> >;

	struct FileMappingReceiver : callbacks::IIndexedFileReceiver
	{
		void receive(FileInfo && fileInfo) override;
		FileSizeMapping_t mapping;
	};

	void calculateHashes(
			FileSizeMapping_t & totalMap,
			DuplicatesList_t & result,
			callbacks::ISearchProcessCallback::Ptr_t searchProcessCallback);
};

}}

#endif /* LOGIC_DUPLICATEFILESFINDER_HPP_ */
