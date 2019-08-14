/*
 ===============================================================================
 Name        : DuplicateImageFinder.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <logic/DuplicateImageFinder.hpp>

#include <logic/callbacks/ImageFinderCallback.hpp>
#include <logic/images/SimilarImageFinder.hpp>
#include <logic/FileIndexer.hpp>


namespace detwinner {
namespace logic {


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
DuplicatesList_t
DuplicateImageFinder::find(
		const std::vector<std::string> & folderList,
		const FileSearchSettings & searchSettings,
		const callbacks::ISearchProcessCallback::Ptr_t & searchProcessCallback)
{
	if (searchProcessCallback) searchProcessCallback->onInit();

	FileToProcessReceiver filesToProcess;
	FileIndexer(searchSettings).performIndexing(folderList, filesToProcess, searchProcessCallback);

	images::DuplicateImageResult res = images::SimilarImageFinder().find(
			filesToProcess.fileNames,
			searchSettings.sensitivity.value_or(kDefaultSensitivity),
			searchSettings.processRotations.value_or(kDefaultProcessRotations),
			std::make_shared<callbacks::ImageFinderCallback>(searchProcessCallback));

	DuplicatesList_t result;
	result.reserve(res.size());

	for (const auto & vec : res)
	{
		DuplicateContainer container;
		const auto imageCount = vec.size();
		container.files.reserve(imageCount);

		for (const images::ImageInfo & imageInfo : vec)
		{
			container.files.emplace_back(
					imageInfo.fileSize,
					imageInfo.fileName,
					imageInfo.width,
					imageInfo.height);
		}
		result.push_back(std::move(container));
	}
	if (searchProcessCallback) searchProcessCallback->onFinish();
	return result;
}


}}
