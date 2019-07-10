#include <stdexcept>
#include <string>
using std::string;
#include <iostream>
using std::ostream;
#include "tree.h"

#ifndef _VALUE_H_
#define _VALUE_H_

extern void RunTimeError (string);
class Value {
public:
    enum ValType { ERRVAL, INTVAL, STRVAL };
    
private:
    ValType        theType;
    int                 theInt;
    string            theStr;
    
public:
    Value() : theType(ERRVAL), theInt(0) {}
    Value(int i) : theType(INTVAL), theInt(i) {}
    Value(string s) : theType(STRVAL), theInt(0) , theStr(s) {}
    
    bool isInt() const { return theType == INTVAL; }
    bool isStr() const { return theType == STRVAL; }
    bool isErr() const { return theType == ERRVAL; }
    
    int GetInt() const {
        if( isInt() ) return theInt;
        throw std::logic_error("This Value is not an int");
    }
    
    string GetStr() const {
        if( isStr() ) return theStr;
        throw std::logic_error("This Value is not a string");
    }
    
    Value operator+(const Value& o) const
    {
        if(this->isInt() && o.isInt())
            return Value(this->GetInt() + o.GetInt()); 
        else if(this->isStr() && o.theType == STRVAL)
            return Value(this->GetStr() + o.GetStr());
       
        RunTimeError("Type mismatch for arguments of +");
        return Value();
    }
    Value operator-(const Value& o) const 
    {
        if(this->isInt() && o.isInt())
            return Value(this->GetInt() - o.GetInt());
        RunTimeError("Type mismatch for arguments of -");
        return Value();
    }
    Value operator*(const Value& o) const
    {
        if(this->isInt() && o.isInt())
            return Value(this->GetInt() * o.GetInt());
        else if(this->isInt() && o.isStr())
        {
            if(this->GetInt() >= 0)
            {
                string str = "";
                for(int i=0; i<this->GetInt(); i++)
                    str = str + o.GetStr();
                return str;
            }
            else if(o.GetInt() == 0)
            {
                string str = "";
                return str;
            }
        }
        else if(this->isStr() && o.isInt())
        {
            if(o.GetInt() >= 0)
            {
                string str = "";
                for(int i=0; i<o.GetInt(); i++)
                    str = str + this->GetStr();
                return str;
            }
            else if(o.GetInt() == 0)
            {
                string str = "";
                return str;
            }
            else if(o.GetInt() < 0)
            {
                RunTimeError("Repetition count less than 0");
                return Value();
            }
        }
        
        RunTimeError("Type mismatch for arguments of *");
        return Value();
    }
    Value operator/(const Value& o) const
    {
        if(this->isInt() && o.isInt())
        {
            if(o.theInt != 0)
                return Value(this->GetInt() / o.GetInt());
            else if(o.theInt == 0)
            {
                RunTimeError("Divide by zero error");
                return Value();
            }
            else
            {
                RunTimeError("Invalid type for arguments of /");
                return Value();
            }
        }
        return Value();
    }
    
    friend ostream& operator<<(ostream& out, const Value& v) {
        switch( v.theType ) {
            case ERRVAL:
                out << "RUNTIME ERROR: " << v.theStr;
                break;
            case INTVAL:
                out << v.theInt;
                break;
            case STRVAL:
                out << v.theStr;
                break;
        }
        
        return out;
    }
    
};

#endif
