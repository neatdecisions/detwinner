/*
 ===============================================================================
 Name        : IIndexedFileReceiver.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018-2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <string>

namespace detwinner::logic::callbacks {

struct IIndexedFileReceiver
{
	struct FileInfo
	{
		FileInfo(const std::string & fullPath, unsigned long long size) : fullPath(fullPath), size(size) {}
		std::string fullPath;
		unsigned long long size;
	};

	virtual void receive(FileInfo && fileInfo) = 0;
	virtual ~IIndexedFileReceiver() noexcept = default;
};

} // namespace detwinner::logic::callbacks
