/*
 ===============================================================================
 Name        : IIndexedFileReceiver.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef LOGIC_CALLBACKS_IINDEXEDFILERECEIVER_HPP_
#define LOGIC_CALLBACKS_IINDEXEDFILERECEIVER_HPP_

#include <string>


namespace detwinner {
namespace logic {
namespace callbacks {


struct IIndexedFileReceiver
{
	struct FileInfo
	{
		FileInfo(const std::string & fullPath, unsigned long long size) :
			fullPath(fullPath), size(size){}
		std::string fullPath;
		unsigned long long size;
	};

	virtual void receive(FileInfo && fileInfo) = 0;
	virtual ~IIndexedFileReceiver() noexcept = default;
};


}}}

#endif /* LOGIC_CALLBACKS_IINDEXEDFILERECEIVER_HPP_ */
