/*
 ===============================================================================
 Name        : SearchSettingsManager.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ==============================================================================
 */

#include <settings/SearchSettingsManager.hpp>

#include <giomm.h>
#include <glibmm/fileutils.h>
#include <glibmm/keyfile.h>
#include <glibmm/miscutils.h>

namespace detwinner::settings {

namespace {
const std::string kGroupName_Global = "Global";
const std::string kGroupName_UI = "UI";
const std::string kGroupName_ExactDuplicates = "ExactDuplicates";
const std::string kGroupName_SimilarImages = "SimilarImages";
const std::string kFieldName_Sensitivity = "sensitivity";
const std::string kFieldName_ProcessRotations = "processRotations";
const std::string kFieldName_IncludedRegexps = "includedRegexps";
const std::string kFieldName_MinFileSize_Enabled = "minFileSize.enabled";
const std::string kFieldName_MinFileSize_Value = "minFileSize.value";
const std::string kFieldName_MinFileSize_Unit = "minFileSize.unit";
const std::string kFieldName_MaxFileSize_Enabled = "maxFileSize.enabled";
const std::string kFieldName_MaxFileSize_Value = "maxFileSize.value";
const std::string kFieldName_MaxFileSize_Unit = "maxFileSize.unit";
const std::string kFieldName_AttributeReadOnly = "attributes.readOnly";
const std::string kFieldName_AttributeHidden = "attributes.hidden";
const std::string kFieldName_AttributeExecutable = "attributes.executable";
const std::string kFieldName_DefaultSearchMode = "defaultSearchMode";
const std::string kFieldName_ShowHiddenFiles = "showHiddenFiles";
} // namespace

//------------------------------------------------------------------------------
SearchSettingsManager::SearchSettingsManager(const std::string & settingsFilePath)
		: m_settingsFilePath(settingsFilePath)
{
	m_searchSettings.similarImagesSettings.filenameRegexps = {
			".*?\\.[jJ][pP][gG]$", ".*?\\.[jJ][pP][eE][gG]$", ".*?\\.[pP][nN][gG]$",
			".*?\\.[gG][iI][fF]$", ".*?\\.[bB][mM][pP]$",     ".*?\\.[tT][iI][fF]$",
			".*?\\.[dD][iI][bB]$", ".*?\\.[pP][cC][xX]$",     ".*?\\.[jJ][pP][eE]$",
	};
}

//------------------------------------------------------------------------------
SearchSettings
SearchSettingsManager::getSearchSettings() const
{
	return m_searchSettings;
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::setSearchSettings(const SearchSettings & value)
{
	m_searchSettings = value;
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::saveSettings() const
{
	try
	{
		const std::string & settingsDirectory = Glib::path_get_dirname(m_settingsFilePath);
		auto pSettingsDirectory = Gio::File::create_for_path(settingsDirectory);
		if (pSettingsDirectory)
		{
			if (!pSettingsDirectory->query_exists()) pSettingsDirectory->make_directory();

			Glib::KeyFile keyFile;

			if (m_searchSettings.searchMode == SearchSettings::SearchMode::SimilarImages)
			{
				keyFile.set_string(kGroupName_Global, kFieldName_DefaultSearchMode, kGroupName_SimilarImages);
			} else
			{
				keyFile.set_string(kGroupName_Global, kFieldName_DefaultSearchMode, kGroupName_ExactDuplicates);
			}

			saveCommonSettings(kGroupName_ExactDuplicates, m_searchSettings.exactDuplicatesSettings, keyFile);
			saveCommonSettings(kGroupName_SimilarImages, m_searchSettings.similarImagesSettings, keyFile);
			keyFile.set_integer(kGroupName_SimilarImages, kFieldName_Sensitivity, m_searchSettings.imageSettings.sensitivity);
			keyFile.set_boolean(kGroupName_SimilarImages, kFieldName_ProcessRotations,
			                    m_searchSettings.imageSettings.processRotations);
			keyFile.set_boolean(kGroupName_UI, kFieldName_ShowHiddenFiles, m_searchSettings.uiSettings.showHiddenFiles);

			keyFile.save_to_file(m_settingsFilePath);
		}
	} catch (const Glib::FileError & e)
	{
		g_warning("%s", e.what().c_str());
	}
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::loadFileSizeSetting(const Glib::KeyFile & settingsFile,
                                           const std::string & groupName,
                                           const std::string & fieldEnabled,
                                           const std::string & fieldValue,
                                           const std::string & fieldUnit,
                                           std::optional<SearchSettings::FileSizeSetting> & value) const
{
	if (settingsFile.has_key(groupName, fieldEnabled) || settingsFile.has_key(groupName, fieldValue) ||
	    settingsFile.has_key(groupName, fieldUnit))
	{
		value = SearchSettings::FileSizeSetting();
		value->enabled = readBoolean(settingsFile, groupName, fieldEnabled, value->enabled);

		if (settingsFile.has_key(groupName, fieldValue))
		{
			try
			{
				value->size = settingsFile.get_uint64(groupName, fieldValue);
			} catch (const Glib::KeyFileError & e)
			{
				g_warning("Error when reading settings: %s", e.what().c_str());
			}
		}
		if (settingsFile.has_key(groupName, fieldUnit))
		{
			try
			{
				value->unit = stringToFileSizeUnit(settingsFile.get_string(groupName, fieldUnit));
			} catch (const Glib::KeyFileError & e)
			{
				g_warning("Error when reading settings: %s", e.what().c_str());
			}
		}
	}
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::saveFileSizeSetting(const std::optional<SearchSettings::FileSizeSetting> & value,
                                           const std::string & groupName,
                                           const std::string & fieldEnabled,
                                           const std::string & fieldValue,
                                           const std::string & fieldUnit,
                                           Glib::KeyFile & settingsFile) const
{
	if (value)
	{
		settingsFile.set_boolean(groupName, fieldEnabled, value->enabled);
		settingsFile.set_uint64(groupName, fieldValue, value->size);
		settingsFile.set_string(groupName, fieldUnit, fileSizeUnitToString(value->unit));
	}
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::loadCommonSettings(const std::string & groupName,
                                          const Glib::KeyFile & settingsFile,
                                          SearchSettings::CommonSettings & settings)
{
	if (settingsFile.has_key(groupName, kFieldName_IncludedRegexps))
	{
		try
		{
			auto includedRegexps = settingsFile.get_string_list(groupName, kFieldName_IncludedRegexps);
			for (auto && regex : includedRegexps)
			{
				settings.filenameRegexps.push_back(regex);
			}
		} catch (const Glib::KeyFileError & e)
		{
			g_warning("Error when reading settings: %s", e.what().c_str());
		}
	}

	loadFileSizeSetting(settingsFile, groupName, kFieldName_MaxFileSize_Enabled, kFieldName_MaxFileSize_Value,
	                    kFieldName_MaxFileSize_Unit, settings.maxFileSize);

	loadFileSizeSetting(settingsFile, groupName, kFieldName_MinFileSize_Enabled, kFieldName_MinFileSize_Value,
	                    kFieldName_MinFileSize_Unit, settings.minFileSize);

	settings.searchReadOnly = readBoolean(settingsFile, groupName, kFieldName_AttributeReadOnly, true);
	settings.searchHidden = readBoolean(settingsFile, groupName, kFieldName_AttributeHidden, false);
	settings.searchExecutable = readBoolean(settingsFile, groupName, kFieldName_AttributeExecutable, true);
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::saveCommonSettings(const std::string & groupName,
                                          const SearchSettings::CommonSettings & settings,
                                          Glib::KeyFile & settingsFile) const
{
	if (!settings.filenameRegexps.empty())
	{
		settingsFile.set_string_list(groupName, kFieldName_IncludedRegexps, settings.filenameRegexps);
	}

	saveFileSizeSetting(settings.maxFileSize, groupName, kFieldName_MaxFileSize_Enabled, kFieldName_MaxFileSize_Value,
	                    kFieldName_MaxFileSize_Unit, settingsFile);

	saveFileSizeSetting(settings.minFileSize, groupName, kFieldName_MinFileSize_Enabled, kFieldName_MinFileSize_Value,
	                    kFieldName_MinFileSize_Unit, settingsFile);

	settingsFile.set_boolean(groupName, kFieldName_AttributeReadOnly, settings.searchReadOnly);
	settingsFile.set_boolean(groupName, kFieldName_AttributeHidden, settings.searchHidden);
	settingsFile.set_boolean(groupName, kFieldName_AttributeExecutable, settings.searchExecutable);
}

//------------------------------------------------------------------------------
void
SearchSettingsManager::loadSettings()
{
	m_searchSettings = SearchSettings();

	auto pSettingsFile = Gio::File::create_for_path(m_settingsFilePath);
	if (pSettingsFile && pSettingsFile->query_exists())
	{
		Glib::KeyFile keyFile;
		keyFile.load_from_file(m_settingsFilePath);

		if (keyFile.has_group(kGroupName_Global) && keyFile.has_key(kGroupName_Global, kFieldName_DefaultSearchMode) &&
		    (keyFile.get_string(kGroupName_Global, kFieldName_DefaultSearchMode) == kGroupName_SimilarImages))
		{
			m_searchSettings.searchMode = SearchSettings::SearchMode::SimilarImages;
		}

		if (keyFile.has_group(kGroupName_UI) && keyFile.has_key(kGroupName_UI, kFieldName_ShowHiddenFiles))
		{
			m_searchSettings.uiSettings.showHiddenFiles =
					readBoolean(keyFile, kGroupName_UI, kFieldName_ShowHiddenFiles, false);
		}

		if (keyFile.has_group(kGroupName_SimilarImages))
		{
			try
			{
				if (keyFile.has_key(kGroupName_SimilarImages, kFieldName_Sensitivity))
				{
					m_searchSettings.imageSettings.sensitivity =
							keyFile.get_integer(kGroupName_SimilarImages, kFieldName_Sensitivity);
				}
			} catch (const Glib::KeyFileError & e)
			{
				g_warning("Error when reading settings: %s", e.what().c_str());
			}

			m_searchSettings.imageSettings.processRotations =
					readBoolean(keyFile, kGroupName_SimilarImages, kFieldName_ProcessRotations, true);

			loadCommonSettings(kGroupName_SimilarImages, keyFile, m_searchSettings.similarImagesSettings);
		}

		if (keyFile.has_group(kGroupName_ExactDuplicates))
		{
			loadCommonSettings(kGroupName_ExactDuplicates, keyFile, m_searchSettings.exactDuplicatesSettings);
		}
	}
}

//------------------------------------------------------------------------------
SearchSettings::FileSizeUnit
SearchSettingsManager::stringToFileSizeUnit(const std::string & val) const
{
	if (val == "GB") return SearchSettings::FileSizeUnit::GB;
	if (val == "MB") return SearchSettings::FileSizeUnit::MB;
	if (val == "KB") return SearchSettings::FileSizeUnit::KB;
	return SearchSettings::FileSizeUnit::B;
}

//------------------------------------------------------------------------------
std::string
SearchSettingsManager::fileSizeUnitToString(SearchSettings::FileSizeUnit value) const
{
	if (value == SearchSettings::FileSizeUnit::GB) return "GB";
	if (value == SearchSettings::FileSizeUnit::MB) return "MB";
	if (value == SearchSettings::FileSizeUnit::KB) return "KB";
	return "B";
}

//------------------------------------------------------------------------------
bool
SearchSettingsManager::readBoolean(const Glib::KeyFile & settingsFile,
                                   const std::string & groupName,
                                   const std::string & fieldName,
                                   bool defaultValue) const
{
	bool result = defaultValue;
	if (settingsFile.has_key(groupName, fieldName))
	{
		try
		{
			result = settingsFile.get_boolean(groupName, fieldName);
		} catch (const Glib::KeyFileError & e)
		{
			g_warning("Error when reading settings: %s", e.what().c_str());
		}
	}
	return result;
}

} // namespace detwinner::settings
