/*
 ===============================================================================
 Name        : DuplicateFilesFinder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/DuplicateFilesFinder.hpp>

#include <logic/FileIndexer.hpp>
#include <logic/MurmurHash.hpp>

#include <unordered_map>


namespace detwinner {
namespace logic {


//==============================================================================
// DuplicateFilesFinder::FileMappingReceiver
//==============================================================================

//------------------------------------------------------------------------------
void
DuplicateFilesFinder::FileMappingReceiver::receive(FileInfo && fileInfo)
{
	mapping[fileInfo.size].push_back(std::move(fileInfo.fullPath));
}



//==============================================================================
// DuplicateFilesFinder
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesList_t
DuplicateFilesFinder::find(
	const std::vector<std::string> & folderList,
	const FileSearchSettings & searchSettings,
	const callbacks::ISearchProcessCallback::Ptr_t & searchProcessCallback)
{
	if (searchProcessCallback) searchProcessCallback->onInit();

	FileMappingReceiver totalMap;
	FileIndexer(searchSettings).performIndexing(folderList, totalMap, searchProcessCallback);

	DuplicatesList_t result;

	unsigned int count = 0;
	for (auto && mapItem: totalMap.mapping)
	{
		count += mapItem.second.size();
	}

	if (searchProcessCallback)
	{
		if (searchProcessCallback->pauseAndStopStatus()) return result;
		searchProcessCallback->onStartComparing(count);
		searchProcessCallback->setStage(1);
	}

	calculateHashes(totalMap.mapping, result, searchProcessCallback);

	if (searchProcessCallback) searchProcessCallback->onFinish();

	return result;
}


//------------------------------------------------------------------------------
void
DuplicateFilesFinder::calculateHashes(
	FileSizeMapping_t & totalMap,
	DuplicatesList_t & result,
	callbacks::ISearchProcessCallback::Ptr_t searchProcessCallback)
{
	std::unordered_map<std::string, std::vector<std::string> > sameSizeDuplicateGroup;
	std::string hash;

	for (auto & aSizeGroup: totalMap)
	{
		if (aSizeGroup.second.size() > 1)
		{
			sameSizeDuplicateGroup.clear();

			for (auto && aFileName: aSizeGroup.second)
			{
				if ( !MurmurHash::GetHash(aFileName, hash) )
				{
					if (searchProcessCallback) searchProcessCallback->onFileProcessed(aSizeGroup.first);
					continue;
				}

				if (searchProcessCallback)
				{
					if (searchProcessCallback->pauseAndStopStatus()) return;
					searchProcessCallback->onFileProcessed(aSizeGroup.first);
				}

				sameSizeDuplicateGroup[hash].push_back(aFileName);
			}

			for (auto & aDuplicateGroup : sameSizeDuplicateGroup)
			{
				if (aDuplicateGroup.second.size() > 1)
				{
					DuplicateContainer container;
					for (auto && fileName: aDuplicateGroup.second)
					{
						container.files.emplace_back(aSizeGroup.first, fileName);
					}
					if (searchProcessCallback)
					{
						const std::size_t fileCount = aDuplicateGroup.second.size();
						const unsigned long long totalSize = fileCount * aSizeGroup.first;
						searchProcessCallback->onDuplicateFound(fileCount, totalSize, totalSize - aSizeGroup.first);
					}

					result.push_back(std::move(container));
				}
			}
		} else
		{
			if (searchProcessCallback)
			{
				searchProcessCallback->onFileProcessed(aSizeGroup.first);
			}
		}
		aSizeGroup.second.clear();
	}
}


}}
