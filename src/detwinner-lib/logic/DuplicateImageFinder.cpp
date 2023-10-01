/*
 ===============================================================================
 Name        : DuplicateImageFinder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/DuplicateImageFinder.hpp>

#include <logic/FileIndexer.hpp>
#include <logic/callbacks/ImageFinderCallback.hpp>
#include <logic/images/SimilarImageFinder.hpp>

namespace detwinner::logic {

//==============================================================================
// DuplicateImageFinder::FileToProcessReceiver
//==============================================================================

//------------------------------------------------------------------------------
void
DuplicateImageFinder::FileToProcessReceiver::receive(FileInfo && fileInfo)
{
	fileNames.push_back(std::move(fileInfo.fullPath));
}

//==============================================================================
// DuplicateImageFinder
//==============================================================================

//------------------------------------------------------------------------------
DuplicatesList
DuplicateImageFinder::find(const std::vector<std::string> & folderList,
                           const FileSearchSettings & searchSettings,
                           const callbacks::ISearchProcessCallback::Ptr & searchProcessCallback)
{
	if (searchProcessCallback) searchProcessCallback->onInit();

	FileToProcessReceiver filesToProcess;
	FileIndexer(searchSettings).performIndexing(folderList, filesToProcess, searchProcessCallback);

	const images::DuplicateImageResult similarImages = images::SimilarImageFinder().find(
			filesToProcess.fileNames, searchSettings.sensitivity.value_or(kDefaultSensitivity),
			searchSettings.processRotations.value_or(kDefaultProcessRotations),
			std::make_shared<callbacks::ImageFinderCallback>(searchProcessCallback));

	const std::size_t resultSize = similarImages.size();
	DuplicatesList result(resultSize);
	for (std::size_t i = 0; i < resultSize; ++i)
	{
		DuplicateContainer & container = result[i];
		container.files.reserve(similarImages[i].size());
		for (const images::ImageInfo & imageInfo : similarImages[i])
		{
			container.files.emplace_back(imageInfo.fileSize, imageInfo.fileName, imageInfo.width, imageInfo.height);
		}
	}
	if (searchProcessCallback) searchProcessCallback->onFinish();
	return result;
}

} // namespace detwinner::logic
