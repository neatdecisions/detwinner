/*
 ===============================================================================
 Name        : MemoryMappedFile.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <tools/MemoryMappedFile.hpp>

#include <glib.h>
#include <glib/gstdio.h>

namespace detwinner::tools {

//==============================================================================
// MemoryMappedFile::Impl
//==============================================================================
class MemoryMappedFile::Impl
{
public:
	explicit Impl(const std::string & fileName) noexcept : m_pMappedFile(nullptr)
	{
		GError * gError = nullptr;
		m_pMappedFile = g_mapped_file_new(fileName.c_str(), false, &gError);
		if (gError)
		{
			g_warning("Failed to open file: %s", gError->message);
			g_error_free(gError);
			m_pMappedFile = nullptr;
		}
	}

	~Impl() noexcept
	{
		if (m_pMappedFile) g_mapped_file_unref(m_pMappedFile);
	}
	const char * buffer() const noexcept
	{
		return m_pMappedFile == nullptr ? nullptr : g_mapped_file_get_contents(m_pMappedFile);
	}
	unsigned long size() const noexcept
	{
		return m_pMappedFile == nullptr ? 0UL : g_mapped_file_get_length(m_pMappedFile);
	}
	bool valid() const noexcept { return m_pMappedFile != nullptr; }

private:
	mutable GMappedFile * m_pMappedFile;
};

//==============================================================================
// MemoryMappedFile
//==============================================================================

//------------------------------------------------------------------------------
MemoryMappedFile::MemoryMappedFile(const std::string & filePath) : m_pImpl(std::make_unique<Impl>(filePath)) {}

//------------------------------------------------------------------------------
MemoryMappedFile::~MemoryMappedFile() noexcept = default;

//------------------------------------------------------------------------------
bool
MemoryMappedFile::valid() const noexcept
{
	return m_pImpl->valid();
}

//------------------------------------------------------------------------------
unsigned long
MemoryMappedFile::size() const noexcept
{
	return m_pImpl->size();
}

//------------------------------------------------------------------------------
const char *
MemoryMappedFile::buffer() const noexcept
{
	return m_pImpl->buffer();
}

} // namespace detwinner::tools
