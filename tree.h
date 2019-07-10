/*
 * parsetree.h
 */

#ifndef TREE_H_
#define TREE_H_

#include <vector>
#include <map>
#include <string>
#include "value.h"
using std::vector;
using std::map;
using std::string;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
class Value;


extern void RunTimeError (string);
static map<string, Value> myMap;
class ParseTree {

protected:	
    int			linenum;
	ParseTree	*left;
	ParseTree	*right;
    
public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}
    
    virtual ~ParseTree() {
		delete left;
		delete right;
	}
    
    
	int GetLinenum() const { return linenum; }
	virtual NodeType GetType() const { return ERRTYPE; }

	int NodeCount() const {
		int nc = 0;
		if( left ) nc += left->NodeCount();
		if( right ) nc += right->NodeCount();
		return nc + 1;
	}

	int LeafCount() const {
		int lc = 0;
		if( left ) lc += left->LeafCount();
		if( right ) lc += right->LeafCount();
		if( left == 0 && right == 0 )
			lc++;
		return lc;
	}
    
	virtual bool IsIdent() const { return false; }
	virtual bool IsString() const { return false; }
	virtual bool IsOp() const { return false; }
	virtual string GetId() const { return ""; }
    virtual Value Eval() const = 0;
    
	int IdentCount() const {
		int cnt = 0;
		if( left ) cnt += left->IdentCount();
		if( right ) cnt += right->IdentCount();
		if( IsIdent() )
			cnt++;
		return cnt;
	}

	int StringCount() const {
		int cnt = 0;
		if( left ) cnt += left->StringCount();
		if( right ) cnt += right->StringCount();
		if( IsString() )
			cnt++;
		return cnt;
	}

	int OpCount() const {
		int cnt = 0;
		if( left ) cnt += left->OpCount();
		if( right ) cnt += right->OpCount();
		if( IsOp() )
			cnt++;
		return cnt;
	}

	int MaxDepth() const {
		int ld = 0, rd = 0;
		if( left ) ld += left->MaxDepth();
		if( right ) rd += right->MaxDepth();

		return (ld > rd ? ld : rd) + 1;
	}
   
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
    
    Value Eval() const {
        left->Eval();
        if(right)
            right->Eval();
        return Value();
    }
    
};

class IfStatement : public ParseTree {
public:
	IfStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
    Value Eval() const {
        Value val = left->Eval();
        if(val.isInt() && val.GetInt() != 0)
            return right->Eval();
        else if(!val.isInt())
        {
            RunTimeError("Conditional is not an integer");
            return Value();
        }
        return Value();
    }
};

class SetStatement : public ParseTree {
	string id;

public:
	SetStatement(int line, string id, ParseTree *expr) : ParseTree(line, expr), id(id) {}
    Value Eval() const {
        Value v = left->Eval();
        myMap[id] = v;    
        return Value();
    }
};

class PrintStatement : public ParseTree {
public:
	PrintStatement(int line, ParseTree *e) : ParseTree(line, e) {}
    Value Eval() const {
        Value val = left->Eval();
        if(val.isInt())
            cout << val.GetInt();
        else if(val.isStr())
            cout << val.GetStr();
        return Value();
    }
};

class LoopStatement : public ParseTree {
public:
	LoopStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
    //while (expr->eval().GetIntValue())
   // stmtlist->eval();
    Value Eval() const { 
        while(left->Eval().GetInt())
        {
            right->Eval();
        }
        return Value();
    }
};

class Addition : public ParseTree {
public:
	Addition(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
	bool IsOp() const { return true; }
    Value Eval() const {
        return left->Eval() + right->Eval();
    }
};

class Subtraction : public ParseTree {
public:
	Subtraction(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
	bool IsOp() const { return true; }
    Value Eval() const {
        return left->Eval() - right->Eval();
    }
};

class Multiplication : public ParseTree {
public:
	Multiplication(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
	bool IsOp() const { return true; }
    Value Eval() const {
        return left->Eval() * right->Eval();
    }
};

class Division : public ParseTree {
public:
	Division(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
	bool IsOp() const { return true; }
    Value Eval() const {
        return left->Eval() / right->Eval();
    }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(int l, int i) : ParseTree(l), val(i) {}
	IConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
    
	NodeType GetType() const { return INTTYPE; }
    Value Eval() const { return Value(val); }
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}

	NodeType GetType() const { return STRTYPE; }
	bool IsString() const { return true; }
    Value Eval() const { return Value(val); }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
    Value Eval() const { 
       // cout<< "MAP IS -------- " << myMap[GetId()];
        if(!myMap.empty() && myMap.count(id))  
            return myMap[id];
        else
        {
            RunTimeError("Symbol " + id + " not defined");
            return Value();
        }
    }
};

#endif /* TREE_H_ */
