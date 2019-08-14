/*
 ============================================================================
 Name        : DuplicateImageFinder.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ============================================================================
 */

#ifndef LOGIC_IMAGES_DUPLICATEIMAGEFINDER_HPP_
#define LOGIC_IMAGES_DUPLICATEIMAGEFINDER_HPP_

#include <string>
#include <vector>
#include <logic/callbacks/IIndexedFileReceiver.hpp>
#include <logic/callbacks/ISearchProcessCallback.hpp>
#include <logic/CommonDataTypes.hpp>
#include <logic/FileSearchSettings.hpp>


namespace detwinner {
namespace logic {


//==============================================================================
// DuplicateImageFinder
//==============================================================================
class DuplicateImageFinder
{
public:
	DuplicatesList_t find(
			const std::vector<std::string> & folderList,
			const FileSearchSettings & searchSettings,
			const callbacks::ISearchProcessCallback::Ptr_t & searchProcessCallback);

private:
	struct FileToProcessReceiver : callbacks::IIndexedFileReceiver
	{
		void receive(FileInfo && fileInfo) override;
		std::vector<std::string> fileNames;
	};

	static constexpr unsigned short kDefaultSensitivity = 85;
	static constexpr bool kDefaultProcessRotations = false;
};


}}

#endif /* LOGIC_IMAGES_DUPLICATEIMAGEFINDER_HPP_ */
