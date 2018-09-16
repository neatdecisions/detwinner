/*
 ===============================================================================
 Name        : PermanentFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef TOOLS_PERMANENTFILEDELETER_HPP_
#define TOOLS_PERMANENTFILEDELETER_HPP_


#include <tools/AbstractFileDeleter.hpp>


namespace detwinner {
namespace tools {


class PermanentFileDeleter: public AbstractFileDeleter
{
public:
	virtual bool removeFile(const std::string & filePath);
};


}}

#endif /* TOOLS_PERMANENTFILEDELETER_HPP_ */
