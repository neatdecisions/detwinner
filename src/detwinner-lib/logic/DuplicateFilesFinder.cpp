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

#include <numeric>
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

	if (searchProcessCallback)
	{
		if (searchProcessCallback->pauseAndStopStatus()) return DuplicatesList_t();
		const unsigned int count = std::accumulate(totalMap.mapping.begin(), totalMap.mapping.end(), 0,
			[](unsigned int val, const FileSizeMapping_t::value_type & el) { return val + el.second.size(); });
		searchProcessCallback->onStartComparing(count);
		searchProcessCallback->setStage(1);
	}

	DuplicatesList_t result = calculateHashes(totalMap.mapping, searchProcessCallback);

	if (searchProcessCallback) searchProcessCallback->onFinish();

	return result;
}


//------------------------------------------------------------------------------
DuplicatesList_t
DuplicateFilesFinder::calculateHashes(
	FileSizeMapping_t & totalMap,
	callbacks::ISearchProcessCallback::Ptr_t searchProcessCallback) const
{
	std::unordered_map<std::string, std::vector<std::string> > sameSizeDuplicateGroup;
	DuplicatesList_t result;
	std::string hash;

	for (auto & aSizeGroup: totalMap)
	{
		if (aSizeGroup.second.size() > 1)
		{
			sameSizeDuplicateGroup.clear();

			for (const auto & aFileName: aSizeGroup.second)
			{
				if ( !MurmurHash::GetHash(aFileName, hash) )
				{
					if (searchProcessCallback) searchProcessCallback->onFileProcessed(aSizeGroup.first);
					continue;
				}

				if (searchProcessCallback)
				{
					if (searchProcessCallback->pauseAndStopStatus()) return result;
					searchProcessCallback->onFileProcessed(aSizeGroup.first);
				}

				sameSizeDuplicateGroup[hash].push_back(aFileName);
			}

			for (const auto & aDuplicateGroup : sameSizeDuplicateGroup)
			{
				if (aDuplicateGroup.second.size() > 1)
				{
					result.emplace_back();
					DuplicateContainer & container = result.back();
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

	return result;
}


}}
