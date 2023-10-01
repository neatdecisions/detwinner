/*
 ===============================================================================
 Name        : MurmurHash.hpp
 Author      : NeatDecisions
 Version     :
 Description : Adaptation of MurmurHash3 was written by Austin Appleby.
 ===============================================================================
 */

#pragma once

#include <string>

namespace detwinner::logic {

bool BuildMurmurHash(const std::string & fileName, std::string & hash);

}
