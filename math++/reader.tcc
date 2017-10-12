///////////////////////////////////////////////////////////////////////
//  Math Type Library
//  $Id: reader.tcc,v 1.8 2002/07/02 00:56:12 cparpart Exp $
//  (This file contains the reader (parser) specific template members)
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
#ifndef libmath_reader_h
#error You may not include math++/reader.tcc directly; include math++/reader.h instead.
#endif

#include <memory>
#include <sstream>
#include <iostream> // just for debuggin

#include <cstring>
#include <cctype>

namespace math {

// TReader<>
template<class T>
TReader<T>::TReader(const std::string& AInput) :
    FExprStr(AInput), FPos(0), FToken(tkInvalid) {
}

template<class T>
TNode<T> *TReader<T>::parse(const std::string& AInput, bool AEquation) {
    TReader<T> reader(AInput);
    TNode<T> *e = AEquation ? reader.equation(true) : reader.expr(true);

    if (reader.FToken != tkEnd) {
        throw EReadError("Unexpected characters left on input (" 
            + tok2str(reader.FToken) + ").");
    }

    return e;
}

template<class T>
TNode<T> *TReader<T>::equation(bool get) {
    TNode<T> *left = simpleExpr(get);

    switch (FToken) {
        case tkEqu:
            left = new TEquNode<T>(left, simpleExpr(true));
            break;
        case tkUnEqu:
            left = new TUnEquNode<T>(left, simpleExpr(true));
            break;
        case tkGreater:
            left = new TGreaterNode<T>(left, simpleExpr(true));
            break;
        case tkLess:
            left = new TLessNode<T>(left, simpleExpr(true));
            break;
        case tkGreaterEqu:
            left = new TGreaterEquNode<T>(left, simpleExpr(true));
            break;
        case tkLessEqu:
            left = new TLessEquNode<T>(left, simpleExpr(true));
            break;
        default:
            // Enforce a syntax error. Because the input is no 
            // equation as requested.
            consume(tkEqu);
    }
    return left;
}

template<class T>
TNode<T> *TReader<T>::expr(bool get) {
    TNode<T> *left = simpleExpr(get);
    
    while (true) {
        switch (FToken) {
            case tkEqu:
                left = new TEquNode<T>(left, simpleExpr(true));
                break;
            case tkUnEqu:
                left = new TUnEquNode<T>(left, simpleExpr(true));
                break;
            case tkGreater:
                left = new TGreaterNode<T>(left, simpleExpr(true));
                break;
            case tkLess:
                left = new TLessNode<T>(left, simpleExpr(true));
                break;
            case tkGreaterEqu:
                left = new TGreaterEquNode<T>(left, simpleExpr(true));
                break;
            case tkLessEqu:
                left = new TLessEquNode<T>(left, simpleExpr(true));
                break;
            default:
                return left;
        }
    }
}

template<class T>
TNode<T> *TReader<T>::simpleExpr(bool get) {
    TNode<T> *left = term(get);

    while (true) {
        switch (FToken) {
            case tkPlus:
                left = new TPlusNode<T>(left, term(true));
                break;
            case tkMinus:
                left = new TPlusNode<T>(left, new TNegNode<T>(term(true)));
                break;
            default:
                return left;
        }
    }
}

template<class T>
TNode<T> *TReader<T>::term(bool get) {
    TNode<T> *left = factor(get);

    while (true) {
        switch (FToken) {
            case tkMul:
                left = new TMulNode<T>(left, factor(true));
                break;
            case tkDiv:
                left = new TDivNode<T>(left, factor(true));
                break;
            case tkSymbol:
            case tkRndOpen:
            case tkBrOpen:
                // "algebraische schreibweise, z.B.: 2x+3, anstatt 2*x+3"
                left = new TMulNode<T>(left, factor(false));
            default:
                return left;
        }
    }
}

template<class T>
TNode<T> *TReader<T>::factor(bool get) {
    TNode<T> *left = prim(get);

    while (true) {
        switch (FToken) {
            case tkPow:
                left = new TPowNode<T>(left, factor(true));
            default:
                return left;
        }
    }
}

template<class T>
TNode<T> *TReader<T>::prim(bool get) {
    TNode<T> *n = 0;

    if (get)
        nextToken();

    switch (FToken) {
        case tkPlus:
            n = prim(true);
            break;
        case tkMinus:
            n = new TNegNode<T>(prim(true));
            break;
        case tkRndOpen:
            n = expr(true);
            consume(tkRndClose);
            break;
        case tkBrOpen:
            n = expr(true);
            consume(tkBrClose);
            break;
        case tkNumber:
            n = new TNumberNode<T>(FNumber);
            nextToken();
            break;
        case tkSymbol:
            n = createSymbol();
            break;
        case tkEnd:
            throw EReadError("Incomplete expression.");
        default:
            throw EReadError("Unexpected token " + tok2str(FToken) + ".");
            ;// unknown or misplaced token? (returns automatically 0)
    }

    return n;
}

template<class T>
TNode<T> *TReader<T>::createSymbol() {
    if (FSymbol == "x") {
        nextToken();
        return new TParamNode<T>();
    } else if (FSymbol == "sin")
        return new TSinNode<T>(param());
    else if (FSymbol == "cos")
        return new TCosNode<T>(param());
    else if (FSymbol == "tan")
        return new TTanNode<T>(param());
    else if (FSymbol == "ln")
        return new TLnNode<T>(param());
    else if (FSymbol == "IF") { // example
        nextToken();

        consume(tkRndOpen);
        TNode<T> *ifExpr = expr(false);
        consume(tkComma);
        TNode<T> *thenExpr = expr(false);
        consume(tkComma);
        TNode<T> *elseExpr = expr(false);
        consume(tkRndClose);

        return new TIfNode<T>(ifExpr, thenExpr, elseExpr);
    } else {
        // okay, it's a user defined function to be called, or any constant 
        // defined in a library
        std::string name(FSymbol);

        if (nextToken() == tkRndOpen) {
#if 0 // TO BE IMPLEMENTED / DONE
            // a function
            TFuncNode *node = new TFuncNode<T>(name);

            node->addParam(expr(true));

            while (FToken == tkComma)
                node->addParam(expr(true));

            consume(tkRndClose);

            return node;
#else
            TNode<T> *param = expr(true);
            consume(tkRndClose);
            return new TFuncNode<T>(name, param);
#endif
        } else
            // a constant
            return new TSymbolNode<T>(name);
    }
}

template<class T>
TNode<T> *TReader<T>::param() {
    consume(tkSymbol);
    consume(tkRndOpen);
    TNode<T> *param = expr(false);
    consume(tkRndClose);

    return param;
}

template<class T>
bool TReader<T>::eof() const {
    return FPos >= FExprStr.size();
}

template<class T>
typename TReader<T>::TToken TReader<T>::nextToken() {
    // reset data
    FToken = tkInvalid;
    FSymbol.erase();

    // skip spaces
#if defined(__GNUC__) && (__GNUC__ < 3)
    while (FPos < FExprStr.size() && isspace(FExprStr[FPos]))
#else
    while (FPos < FExprStr.size() && std::isspace(FExprStr[FPos]))
#endif
        ++FPos;

    if (eof())
        return FToken = tkEnd;

    if (readOperator())
        return FToken; // readOperator sets FToken automatically

    if (readNumber())
        return FToken = tkNumber;

    if (readSymbol())
        return FToken = tkSymbol;

    return FToken;
}

template<class T>
bool TReader<T>::readOperator() {
    if (std::strchr("+-*/^(,)[]=", FExprStr[FPos])) {
        FToken = TToken(FExprStr[FPos++]);
        return true;
    }

    switch (FExprStr[FPos]) {
        case '<':
            switch (FExprStr[++FPos]) {
                case '=':
                    ++FPos;
                    return FToken = tkLessEqu;
                case '>':
                    ++FPos;
                    return FToken = tkUnEqu;
                default:
                    return FToken = tkLess;
            }
        case '>':
            if (FExprStr[++FPos] == tkEqu) {
                ++FPos;
                return FToken = tkGreaterEqu;
            }
            return FToken = tkGreater;
        default:
            return false;
    }
}

template<class T>
bool TReader<T>::readNumber() {
#if defined(__GNUC__) && (__GNUC__ < 3)
    while (isdigit(FExprStr[FPos]) || FExprStr[FPos] == '.')
#else
    while (std::isdigit(FExprStr[FPos]) || FExprStr[FPos] == '.')
#endif
        FSymbol += FExprStr[FPos++];

    if (FSymbol.size()) {
        std::stringstream sstr(FSymbol);
        sstr >> FNumber;

        return true;
    }
    return false;
}

template<class T>
bool TReader<T>::readSymbol() {
#if defined(__GNUC__) && (__GNUC__ < 3)
    while (FPos < FExprStr.size() && isalpha(FExprStr[FPos]))
#else
    while (FPos < FExprStr.size() && std::isalpha(FExprStr[FPos]))
#endif
        FSymbol += FExprStr[FPos++];

    return FSymbol.size();
}

template<class T>
std::string TReader<T>::tok2str(TToken AToken) {
    // The TToken structure is optimized for parsing, so it can't be
    // optimized here. We're using a switch instead;)
    switch (AToken) {
        case tkEqu:         return "=";
        case tkUnEqu:       return "<>";
        case tkLess:        return "<";
        case tkGreater:     return ">";
        case tkLessEqu:     return "<=";
        case tkGreaterEqu:  return ">=";
        case tkVeryLess:    return "<<";
        case tkVeryGreat:   return ">>";
        case tkPlus:        return "+";
        case tkMinus:       return "-";
        case tkMul:         return "*";
        case tkDiv:         return "/";
        case tkPow:         return "^";
        case tkRndOpen:     return "(";
        case tkRndClose:    return ")";
        case tkBrOpen:      return "[";
        case tkBrClose:     return "]";
        case tkAngOpen:     return "<";
        case tkAngClose:    return ">";
        case tkSetOpen:     return "{";
        case tkSetClose:    return "}";
        case tkNumber:      return "<number>";
        case tkSymbol:      return "<symbol>";
        case tkEnd:         return "<end of input>";
        case tkInvalid:     return "<invalid input>";
        default:            return "<unknown token-id>";
    }
}

template<class T>
void TReader<T>::consume(TToken AToken) {
    if (FToken != AToken)
        throw EReadError(
            std::string("Expected token was ") +
            tok2str(AToken) +
            std::string(" but got ") +
            tok2str(FToken) +
            std::string(" instead.")
        );

    nextToken();
}

} // namespace math
