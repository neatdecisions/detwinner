#include <gtest/gtest.h>

#include <settings/SearchSettingsManager.hpp>


namespace detwinner {
namespace settings {


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, default)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	const auto searchSettings = settingsManager.getSearchSettings();
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, searchSettings.searchMode);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchExecutable);

	EXPECT_EQ(85, searchSettings.imageSettings.sensitivity);
	EXPECT_TRUE(searchSettings.imageSettings.processRotations);
	EXPECT_FALSE(searchSettings.similarImagesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.similarImagesSettings.maxFileSize);
	ASSERT_EQ(9UL, searchSettings.similarImagesSettings.filenameRegexps.size());
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[0], ".*?\\.[jJ][pP][gG]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[1], ".*?\\.[jJ][pP][eE][gG]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[2], ".*?\\.[pP][nN][gG]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[3], ".*?\\.[gG][iI][fF]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[4], ".*?\\.[bB][mM][pP]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[5], ".*?\\.[tT][iI][fF]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[6], ".*?\\.[dD][iI][bB]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[7], ".*?\\.[pP][cC][xX]$");
	EXPECT_EQ(searchSettings.similarImagesSettings.filenameRegexps[8], ".*?\\.[jJ][pP][eE]$");
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.similarImagesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, settings_ini)
{
	SearchSettingsManager settingsManager("data/settings/settings.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings();
	EXPECT_EQ(SearchSettings::SearchMode_t::kSimilarImages, searchSettings.searchMode);
	EXPECT_EQ(80, searchSettings.imageSettings.sensitivity);
	EXPECT_FALSE(searchSettings.imageSettings.processRotations);
	EXPECT_TRUE(searchSettings.similarImagesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.similarImagesSettings.minFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.similarImagesSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.similarImagesSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.similarImagesSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.similarImagesSettings.maxFileSize.value().enabled);
	EXPECT_EQ(1ULL, searchSettings.similarImagesSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kMB, searchSettings.similarImagesSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.similarImagesSettings.filenameRegexps.empty());
	EXPECT_FALSE(searchSettings.similarImagesSettings.searchReadOnly);
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchExecutable);

	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.minFileSize.value().enabled);
	EXPECT_EQ(150ULL, searchSettings.exactDuplicatesSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kKB, searchSettings.exactDuplicatesSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.maxFileSize);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.maxFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.exactDuplicatesSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.exactDuplicatesSettings.maxFileSize.value().unit);
	ASSERT_EQ(1ULL, searchSettings.exactDuplicatesSettings.filenameRegexps.size());
	EXPECT_EQ(".*?", searchSettings.exactDuplicatesSettings.filenameRegexps[0]);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchExecutable);
}


//------------------------------------------------------------------------------
TEST(SearchSettingsManagerTest, corrupted_ini)
{
	SearchSettingsManager settingsManager("data/settings/corrupted.ini");
	settingsManager.loadSettings();
	const auto searchSettings = settingsManager.getSearchSettings();
	EXPECT_EQ(SearchSettings::SearchMode_t::kExactDuplicates, searchSettings.searchMode);
	EXPECT_EQ(85, searchSettings.imageSettings.sensitivity);
	EXPECT_TRUE(searchSettings.imageSettings.processRotations);
	EXPECT_TRUE(searchSettings.similarImagesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.similarImagesSettings.minFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.similarImagesSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kKB, searchSettings.similarImagesSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.similarImagesSettings.maxFileSize);
	EXPECT_TRUE(searchSettings.similarImagesSettings.maxFileSize.value().enabled);
	EXPECT_EQ(-1, searchSettings.similarImagesSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.similarImagesSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.similarImagesSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchReadOnly);
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.similarImagesSettings.searchExecutable);

	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.minFileSize);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.minFileSize.value().enabled);
	EXPECT_EQ(150ULL, searchSettings.exactDuplicatesSettings.minFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.exactDuplicatesSettings.minFileSize.value().unit);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.maxFileSize);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.maxFileSize.value().enabled);
	EXPECT_EQ(0ULL, searchSettings.exactDuplicatesSettings.maxFileSize.value().size);
	EXPECT_EQ(SearchSettings::FileSizeUnit_t::kB, searchSettings.exactDuplicatesSettings.maxFileSize.value().unit);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.filenameRegexps.empty());
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchReadOnly);
	EXPECT_FALSE(searchSettings.exactDuplicatesSettings.searchHidden);
	EXPECT_TRUE(searchSettings.exactDuplicatesSettings.searchExecutable);
}


}}
