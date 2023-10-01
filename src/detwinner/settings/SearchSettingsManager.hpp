/*
 ===============================================================================
 Name        : SearchSettingsManager.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <settings/SearchSettings.hpp>

namespace Glib {
class KeyFile;
}

namespace detwinner::settings {

class SearchSettingsManager
{
public:
	explicit SearchSettingsManager(const std::string & settingsFilePath);

	void saveSettings() const;
	void loadSettings();

	SearchSettings getSearchSettings() const;
	void setSearchSettings(const SearchSettings & value);

private:
	void loadCommonSettings(const std::string & groupName,
	                        const Glib::KeyFile & settingsFile,
	                        SearchSettings::CommonSettings & settings);

	void saveCommonSettings(const std::string & groupName,
	                        const SearchSettings::CommonSettings & settings,
	                        Glib::KeyFile & settingsFile) const;

	void loadFileSizeSetting(const Glib::KeyFile & settingsFile,
	                         const std::string & groupName,
	                         const std::string & fieldEnabled,
	                         const std::string & fieldValue,
	                         const std::string & fieldUnit,
	                         std::optional<SearchSettings::FileSizeSetting> & value) const;

	void saveFileSizeSetting(const std::optional<SearchSettings::FileSizeSetting> & value,
	                         const std::string & groupName,
	                         const std::string & fieldEnabled,
	                         const std::string & fieldValue,
	                         const std::string & fieldUnit,
	                         Glib::KeyFile & settingsFile) const;

	SearchSettings::FileSizeUnit stringToFileSizeUnit(const std::string & val) const;
	std::string fileSizeUnitToString(SearchSettings::FileSizeUnit value) const;

	bool readBoolean(const Glib::KeyFile & settingsFile,
	                 const std::string & groupName,
	                 const std::string & fieldName,
	                 bool defaultValue) const;

	SearchSettings m_searchSettings;
	const std::string m_settingsFilePath;
};

} // namespace detwinner::settings