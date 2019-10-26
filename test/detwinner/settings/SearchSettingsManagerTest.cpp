#include <gtest/gtest.h>

#include <settings/SearchSettingsManager.hpp>


namespace detwinner {
namespace settings {


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, default_exact_duplicates)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kExactDuplicates);
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, searchSettings.searchMode);
	EXPECT_FALSE(searchSettings.imageSettings);
	EXPECT_FALSE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.searchHidden);
	EXPECT_FALSE(searchSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, default_similar_images)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kSimilarImages);
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, searchSettings.searchMode);
	ASSERT_TRUE(searchSettings.imageSettings);
	EXPECT_EQ(85, searchSettings.imageSettings.value().sensitivity);
	EXPECT_TRUE(searchSettings.imageSettings.value().processRotations);
	EXPECT_FALSE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.maxFileSize);
	ASSERT_EQ(9UL, searchSettings.filenameRegexps.size());
	EXPECT_EQ(searchSettings.filenameRegexps[0], ".*?\\.[jJ][pP][gG]$");
	EXPECT_EQ(searchSettings.filenameRegexps[1], ".*?\\.[jJ][pP][eE][gG]$");
	EXPECT_EQ(searchSettings.filenameRegexps[2], ".*?\\.[pP][nN][gG]$");
	EXPECT_EQ(searchSettings.filenameRegexps[3], ".*?\\.[gG][iI][fF]$");
	EXPECT_EQ(searchSettings.filenameRegexps[4], ".*?\\.[bB][mM][pP]$");
	EXPECT_EQ(searchSettings.filenameRegexps[5], ".*?\\.[tT][iI][fF]$");
	EXPECT_EQ(searchSettings.filenameRegexps[6], ".*?\\.[dD][iI][bB]$");
	EXPECT_EQ(searchSettings.filenameRegexps[7], ".*?\\.[pP][cC][xX]$");
	EXPECT_EQ(searchSettings.filenameRegexps[8], ".*?\\.[jJ][pP][eE]$");
	EXPECT_TRUE(searchSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.searchHidden);
	EXPECT_FALSE(searchSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, settings_ini_similar)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kSimilarImages);
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, searchSettings.searchMode);
	ASSERT_TRUE(searchSettings.imageSettings);
	EXPECT_EQ(80, searchSettings.imageSettings.value().sensitivity);
	EXPECT_FALSE(searchSettings.imageSettings.value().processRotations);
	EXPECT_TRUE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.minFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.maxFileSize.value().enabled);
	EXPECT_EQ(1ULL, searchSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kMB, searchSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.filenameRegexps.empty());
	EXPECT_FALSE(searchSettings.searchReadOnly);
	EXPECT_TRUE(searchSettings.searchHidden);
	EXPECT_TRUE(searchSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, settings_ini_exact)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kExactDuplicates);
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, searchSettings.searchMode);
	EXPECT_FALSE(searchSettings.imageSettings);
	EXPECT_TRUE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.minFileSize.value().enabled);
	EXPECT_EQ(150ULL, searchSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kKB, searchSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.maxFileSize);
	EXPECT_FALSE(searchSettings.maxFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.maxFileSize.value().unit);
	ASSERT_EQ(1ULL, searchSettings.filenameRegexps.size());
	EXPECT_EQ(".*?", searchSettings.filenameRegexps[0]);
	EXPECT_TRUE(searchSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.searchHidden);
	EXPECT_TRUE(searchSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, corrupted_ini_similar)
{
	SearchSettingsManager settingsManager("data/settings/corrupted.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kSimilarImages);
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, searchSettings.searchMode);
	ASSERT_TRUE(searchSettings.imageSettings);
	EXPECT_EQ(85, searchSettings.imageSettings.value().sensitivity);
	EXPECT_TRUE(searchSettings.imageSettings.value().processRotations);
	EXPECT_TRUE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.minFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kKB, searchSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.maxFileSize.value().enabled);
	EXPECT_EQ(-1, searchSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.searchReadOnly);
	EXPECT_TRUE(searchSettings.searchHidden);
	EXPECT_FALSE(searchSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, corrupted_ini_exact)
{
	SearchSettingsManager settingsManager("data/settings/corrupted.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings(SearchSettings::SearchMode_t::kExactDuplicates);
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, settingsManager.getDefaultMode());
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, searchSettings.searchMode);
	EXPECT_FALSE(searchSettings.imageSettings);
	EXPECT_TRUE(searchSettings.minFileSize);
	EXPECT_FALSE(searchSettings.minFileSize.value().enabled);
	EXPECT_EQ(150ULL, searchSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.maxFileSize);
	EXPECT_FALSE(searchSettings.maxFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.searchHidden);
	EXPECT_TRUE(searchSettings.searchExecutable);
}


}}
