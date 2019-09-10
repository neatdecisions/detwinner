/*
 ===============================================================================
 Name        : MemoryMappedFile.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef TOOLS_MEMORYMAPPEDFILE_HPP_
#define TOOLS_MEMORYMAPPEDFILE_HPP_

#include <string>
#include <memory>


namespace detwinner {
namespace tools {


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


}}

#endif /* TOOLS_MEMORYMAPPEDFILE_HPP_ */
