/*
 ===============================================================================
 Name        : SearchSettingsManager.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef SETTINGS_SEARCHSETTINGSMANAGER_HPP_
#define SETTINGS_SEARCHSETTINGSMANAGER_HPP_

#include <settings/SearchSettings.hpp>


namespace Glib {
	class KeyFile;
}


namespace detwinner {
namespace settings {


class SearchSettingsManager
{
public:
	explicit SearchSettingsManager(const std::string & settingsFilePath);

	void saveSettings() const;
	void loadSettings();

	SearchSettings getSearchSettings(SearchSettings::SearchMode_t mode) const;
	void setSearchSettings(const SearchSettings & value);

	SearchSettings::SearchMode_t getDefaultMode() const;
	void setDefaultMode(SearchSettings::SearchMode_t value);

private:
	SearchSettings createDefaultSimilarImagesSettings() const;
	SearchSettings createDefaultExactDuplicatesSettings() const;
	void loadCommonSettings(
			const std::string & groupName,
			const Glib::KeyFile & settingsFile,
			SearchSettings & settings);

	void saveCommonSettings(
			const std::string & groupName,
			const SearchSettings & settings,
			Glib::KeyFile & settingsFile) const;

	void loadFileSizeSetting(
			const Glib::KeyFile & settingsFile,
			const std::string & groupName,
			const std::string & fieldEnabled,
			const std::string & fieldValue,
			const std::string & fieldUnit,
			stdx::optional<SearchSettings::FileSizeSetting_t> & value) const;

	void saveFileSizeSetting(
			const stdx::optional<SearchSettings::FileSizeSetting_t> & value,
			const std::string & groupName,
			const std::string & fieldEnabled,
			const std::string & fieldValue,
			const std::string & fieldUnit,
			Glib::KeyFile & settingsFile) const;

	SearchSettings::FileSizeUnit_t stringToFileSizeUnit(const std::string & val) const;
	std::string fileSizeUnitToString(SearchSettings::FileSizeUnit_t value) const;

	bool readBoolean(
			const Glib::KeyFile & settingsFile,
			const std::string & groupName,
			const std::string & fieldName,
			bool defaultValue) const;

	SearchSettings m_exactDuplicatesSettings;
	SearchSettings m_similarImagesSettings;
	const std::string m_settingsFilePath;
	SearchSettings::SearchMode_t m_defaultMode;


	static const std::string kGroupName_Global;
	static const std::string kGroupName_ExactDuplicates;
	static const std::string kGroupName_SimilarImages;
	static const std::string kFieldName_Sensitivity;
	static const std::string kFieldName_ProcessRotations;
	static const std::string kFieldName_IncludedRegexps;
	static const std::string kFieldName_MinFileSize_Enabled;
	static const std::string kFieldName_MinFileSize_Value;
	static const std::string kFieldName_MinFileSize_Unit;
	static const std::string kFieldName_MaxFileSize_Value;
	static const std::string kFieldName_MaxFileSize_Unit;
	static const std::string kFieldName_MaxFileSize_Enabled;
	static const std::string kFieldName_AttributeReadOnly;
	static const std::string kFieldName_AttributeHidden;
	static const std::string kFieldName_AttributeExecutable;
	static const std::string kFieldName_DefaultSearchMode;
	static constexpr unsigned short kDefaultSensitivity = 85;
};


}}

#endif /* SETTINGS_SEARCHSETTINGSMANAGER_HPP_ */
