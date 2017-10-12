///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: library.tcc,v 1.5 2002/04/29 19:34:30 cparpart Exp $
//  (module info here)
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
#include <math++/nodes.h>
#include <math++/reader.h>
#include <math++/printer.h>
#include <math++/calculator.h>

#include <iostream>

namespace math {

///////////////////////////////////////////////////////////////////////
// TFunction<>                                                       //
///////////////////////////////////////////////////////////////////////

template<typename T>
TFunction<T>::TFunction() : 
    FName("f"), FExpression(0) {
}

template<typename T>
TFunction<T>::TFunction(const TFunction& ACopy) : 
    FName(ACopy.FName), FExpression(ACopy.FExpression->clone()) {
}

template<typename T>
TFunction<T>::TFunction(const std::string& AName, const std::string& AExprStr) :
    FName(AName), FExpression(0) {

    expression(AExprStr);
}

template<typename T>
TFunction<T>::TFunction(const std::string& AName, const TNode<T> *ACopyOf) :
    FName(AName), FExpression(ACopyOf->clone()) {
}

template<typename T>
TFunction<T>::~TFunction() {
    // FExpression is either 0 or contains valid allocated data
    // so we may delete it without any check.
    delete FExpression;
}

template<typename T>
T TFunction<T>::call(const T& AParam, const TLibrary<T>& ALibrary, 
    unsigned ALimit) const {

    return TCalculator<T>::calculate(*this, AParam, ALibrary, ALimit);
}

template<typename T>
void TFunction<T>::name(const std::string& AName) {
    FName = AName;
}

template<typename T>
std::string TFunction<T>::name() const {
    return FName;
}

template<typename T>
void TFunction<T>::expression(const TNode<T> *ACopyOf) {
    delete FExpression;
    FExpression = ACopyOf->clone();
}

template<typename T>
void TFunction<T>::expression(const std::string& AExprStr) {
    delete FExpression;

    try {
        FExpression = math::TReader<double>::parse(AExprStr);
    } catch (...) {
        // set FExpression to zero if parser has thrown...
        FExpression = 0;
        throw;
    }
}

template<typename T>
TNode<T> *TFunction<T>::expression() const {
    return FExpression;
}

///////////////////////////////////////////////////////////////////////
// TConstant<>                                                       //
///////////////////////////////////////////////////////////////////////

template<typename T>
TConstant<T>::TConstant() : FName("c"), FValue(1) {
}

template<typename T>
TConstant<T>::TConstant(const TConstant<T>& ACopyOf) : 
    FName(ACopyOf.FName), FValue(ACopyOf.FValue) {
}

template<typename T>
TConstant<T>::TConstant(const std::string& AName, const T& AValue) :
    FName(AName), FValue(AValue) {
}

template<typename T>
void TConstant<T>::name(const std::string& AName) {
    FName = AName;
}

template<typename T>
std::string TConstant<T>::name() const {
    return FName;
}

template<typename T>
void TConstant<T>::value(const T& AValue) {
    FValue = AValue;
}

template<typename T>
T TConstant<T>::value() const {
    return FValue;
}

///////////////////////////////////////////////////////////////////////
// TLibrary<>                                                        //
///////////////////////////////////////////////////////////////////////

template<typename T>
TLibrary<T>::TLibrary() {
}

template<typename T>
TLibrary<T>::TLibrary(const TLibrary<T>& ACopyOf) :
    FFunctions(ACopyOf.FFunctions),
    FConstants(ACopyOf.FConstants) {
}

template<typename T>
void TLibrary<T>::removeIf(const std::string& AName, bool AReplaceIfExists) {
    for (typename TFunctionList::iterator i = FFunctions.begin(); i != FFunctions.end(); ++i) {
        if (i->name() == AName) {
            if (AReplaceIfExists) {
                FFunctions.erase(i);
                break;
            } else
                throw ELibraryLookup("Can't insert multiple elements with same name: " + AName + ".");
        }
    }

    for (typename TConstantList::iterator i = FConstants.begin(); i != FConstants.end(); ++i) {
        if (i->name() == AName) {
            if (AReplaceIfExists) {
                FConstants.erase(i);
                break;
            } else
                throw ELibraryLookup("Can't insert multiple elements with same name: " + AName + ".");
        }
    }
}

template<typename T>
void TLibrary<T>::insert(const TFunction<T>& AFunc, bool AReplaceIfExists) {
    removeIf(AFunc.name(), AReplaceIfExists);

    FFunctions.push_back(AFunc);
}

template<typename T>
void TLibrary<T>::insert(const TConstant<T>& AConst, bool AReplaceIfExists) {
    removeIf(AConst.name(), AReplaceIfExists);

    FConstants.push_back(AConst);
}

template<typename T>
void TLibrary<T>::remove(const std::string& AName) {
    for (typename TFunctionList::iterator i = FFunctions.begin(); i != FFunctions.end(); ++i)
        if (i->name() == AName) {
            FFunctions.erase(i);
            return;
        }

    for (typename TConstantList::iterator i = FConstants.begin(); i != FConstants.end(); ++i)
        if (i->name() == AName) {
            FConstants.erase(i);
            return;
        }

    throw ELibraryLookup("No element found in library called: " + AName + ".");
}

template<typename T>
TFunction<T> TLibrary<T>::function(const std::string& AName) const {
    for (typename TFunctionList::const_iterator i = FFunctions.begin(); i != FFunctions.end(); ++i)
        if (i->name() == AName)
            return *i;

    throw ELibraryLookup("No function found in library called: " + AName + ".");
}

template<typename T>
TConstant<T> TLibrary<T>::constant(const std::string& AName) const {
    for (typename TConstantList::const_iterator i = FConstants.begin(); i != FConstants.end(); ++i)
        if (i->name() == AName)
            return *i;

    throw ELibraryLookup("No constant found in library called: " + AName + ".");
}

template<typename T>
bool TLibrary<T>::hasFunction(const std::string& AName) const {
    for (typename TFunctionList::const_iterator i = FFunctions.begin(); i != FFunctions.end(); ++i)
        if (i->name() == AName)
            return true;

    return false;
}

template<typename T>
bool TLibrary<T>::hasConstant(const std::string& AName) const {
    for (typename TConstantList::const_iterator i = FConstants.begin(); i != FConstants.end(); ++i)
        if (i->name() == AName)
            return true;

    return false;
}

template<typename T>
T TLibrary<T>::call(const std::string& AName, const T& AParam) const {
    for (typename TFunctionList::const_iterator i = FFunctions.begin(); i != FFunctions.end(); ++i)
        if (i->name() == AName)
            return i->call(AParam, *this);

    throw ELibraryLookup("No function found in library called: " + AName + ".");
}

template<typename T>
T TLibrary<T>::value(const std::string& AName) const {
    for (typename TConstantList::const_iterator i = FConstants.begin(); i != FConstants.end(); ++i)
        if (i->name() == AName)
            return i->value();

    throw ELibraryLookup("No constant found in library called: " + AName + ".");
}

template<typename T>
unsigned TLibrary<T>::functions() const {
    return FFunctions.size();
}

template<typename T>
unsigned TLibrary<T>::constants() const {
    return FConstants.size();
}

} // namespace math

template<typename T> 
std::ostream& operator<<(std::ostream& AOut, const math::TFunction<T>& AFunc) {
    AOut << AFunc.name()
         << "(x)=" 
         << math::TPrinter<double>::print(AFunc.expression());

    return AOut;
}
