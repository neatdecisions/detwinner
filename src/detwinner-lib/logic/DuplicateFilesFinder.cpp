/*
 ===============================================================================
 Name        : DuplicateFilesFinder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/DuplicateFilesFinder.hpp>

#include <numeric>
#include <unordered_map>

#include <logic/FileIndexer.hpp>
#include <logic/MurmurHash.hpp>

namespace detwinner::logic {

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
DuplicatesList
DuplicateFilesFinder::find(const std::vector<std::string> & folderList,
                           const FileSearchSettings & searchSettings,
                           const callbacks::ISearchProcessCallback::Ptr & searchProcessCallback)
{
	if (searchProcessCallback) searchProcessCallback->onInit();

	FileMappingReceiver totalMap;
	FileIndexer(searchSettings).performIndexing(folderList, totalMap, searchProcessCallback);

	if (searchProcessCallback)
	{
		if (searchProcessCallback->pauseAndStopStatus()) return DuplicatesList();
		const unsigned int count = std::accumulate(
				totalMap.mapping.begin(), totalMap.mapping.end(), 0,
				[](unsigned int val, const FileSizeMapping::value_type & el) { return val + el.second.size(); });
		searchProcessCallback->onStartComparing(count);
		searchProcessCallback->setStage(1);
	}

	DuplicatesList result = calculateHashes(totalMap.mapping, searchProcessCallback);

	if (searchProcessCallback) searchProcessCallback->onFinish();

	return result;
}

//------------------------------------------------------------------------------
DuplicatesList
DuplicateFilesFinder::calculateHashes(FileSizeMapping & totalMap,
                                      callbacks::ISearchProcessCallback::Ptr searchProcessCallback) const
{
	std::unordered_map<std::string, std::vector<std::string>> sameSizeDuplicateGroup;
	DuplicatesList result;
	std::string hash;

	for (auto & sizeGroup : totalMap)
	{
		if (sizeGroup.second.size() > 1)
		{
			sameSizeDuplicateGroup.clear();

			for (const auto & fileName : sizeGroup.second)
			{
				if (!BuildMurmurHash(fileName, hash))
				{
					if (searchProcessCallback) searchProcessCallback->onFileProcessed(sizeGroup.first);
					continue;
				}

				if (searchProcessCallback)
				{
					if (searchProcessCallback->pauseAndStopStatus()) return result;
					searchProcessCallback->onFileProcessed(sizeGroup.first);
				}

				sameSizeDuplicateGroup[hash].push_back(fileName);
			}

			for (const auto & duplicateGroup : sameSizeDuplicateGroup)
			{
				if (duplicateGroup.second.size() > 1)
				{
					result.emplace_back();
					DuplicateContainer & container = result.back();
					for (auto && fileName : duplicateGroup.second)
					{
						container.files.emplace_back(sizeGroup.first, fileName);
					}
					if (searchProcessCallback)
					{
						const std::size_t fileCount = duplicateGroup.second.size();
						const unsigned long long totalSize = fileCount * sizeGroup.first;
						searchProcessCallback->onDuplicateFound(fileCount, totalSize, totalSize - sizeGroup.first);
					}
				}
			}
		} else
		{
			if (searchProcessCallback)
			{
				searchProcessCallback->onFileProcessed(sizeGroup.first);
			}
		}
		sizeGroup.second.clear();
	}

	return result;
}

} // namespace detwinner::logic
