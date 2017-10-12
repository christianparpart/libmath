///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: error.h,v 1.2 2002/07/02 03:24:27 cparpart Exp $
//  (This file contains the error specific interface for libmath)
//
//  Copyright (c) 2002 by Christian Parpart <cparpart@surakware.net>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
// 
//  You should have received a copy of the GNU Library General Public License
//  along with this library; see the file COPYING.LIB.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.
///////////////////////////////////////////////////////////////////////
#ifndef libmath_error_h
#define libmath_error_h

#include <string>

namespace math {

/**
  * EMath is the base class for all libmath++ error classes.
  * The derivations of EMath may be found in its corresponding modules.
  */
class EMath {
private:
    std::string FReason;

public:
    EMath(const std::string& AReason) : FReason(AReason) {}
    
    std::string reason() const { return FReason; }
};

} // namespace math

#endif
