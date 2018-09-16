/*
 ===============================================================================
 Name        : MurmurHash.hpp
 Author      : NeatDecisions
 Version     :
 Description : Adaptation of MurmurHash3 was written by Austin Appleby.
 ===============================================================================
 */

#ifndef LOGIC_MURMURHASH_HPP_
#define LOGIC_MURMURHASH_HPP_

#include <string>


namespace detwinner {
namespace logic {


class MurmurHash
{
public:

	MurmurHash() = delete;
	~MurmurHash() = delete;

	static bool GetHash(const std::string & fileName, std::string & hash);
};


}}

#endif /* LOGIC_MURMURHASH_HPP_ */
