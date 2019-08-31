/*
 ===============================================================================
 Name        : TrashFileDeleter.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2019 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef TOOLS_TRASHFILEDELETER_HPP_
#define TOOLS_TRASHFILEDELETER_HPP_

#include <tools/AbstractFileDeleter.hpp>


namespace detwinner {
namespace tools {


class TrashFileDeleter: public AbstractFileDeleter
{
public:
	virtual bool removeFile(const std::string & filePath) override;
};


}}

#endif /* TOOLS_TRASHFILEDELETER_HPP_ */
