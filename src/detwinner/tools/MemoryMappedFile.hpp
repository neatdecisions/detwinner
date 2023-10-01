/*
 ===============================================================================
 Name        : MemoryMappedFile.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <memory>
#include <string>

namespace detwinner::tools {

class MemoryMappedFile
{
public:
	explicit MemoryMappedFile(const std::string & filePath);
	~MemoryMappedFile() noexcept;
	bool valid() const noexcept;
	unsigned long size() const noexcept;
	const char * buffer() const noexcept;

private:
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

} // namespace detwinner::tools
