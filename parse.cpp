#include "parse.h"
#include "gettoken.h"

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line+1 << ": " << msg << endl;
}

// Prog is an Slist
ParseTree *Prog(istream& in, int& line)
{
	ParseTree *sl = Slist(in, line);
    
    if( GetToken::Get(in, line) != DONE )
        ParseError(line, "Unrecognized statement");

	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Ssep { Slist } | Stmt Ssep { Slist }
ParseTree *Slist(istream& in, int& line) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;

	Token t;
	if( (t = GetToken::Get(in, line)) != SC && t != NL ) {
		ParseError(line, "Missing statement separator");
		return 0;
	}

    return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream& in, int& line) {
	ParseTree *s = 0;

	Token t = GetToken::Get(in, line);
	switch( t.GetTokenType() ) {
	case IF:
		s = IfStmt(in, line);
		break;

	case PRINT:
		s = PrintStmt(in, line);
		break;

	case SET:
		s = SetStmt(in, line);
		break;

	case LOOP:
		s = LoopStmt(in, line);
		break;

	case ERR:
		ParseError(line, "Invalid token");
		break;

	case DONE:
		break;

	case NL:
	case SC:
		// an empty statement; get the next one
		s = Stmt(in, line);
		break;

	default:
        GetToken::PushBack(t);
        break;
	}

	return s;
}

ParseTree *IfStmt(istream& in, int& line) {
	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after if");
		return 0;
	}

	Token t = GetToken::Get(in, line);

	if( t != BEGIN ) {
		ParseError(line, "Missing BEGIN after expression");
		return 0;
	}

	ParseTree *stmt = Slist(in, line);
	if( stmt == 0 ) {
		ParseError(line, "Missing statement list after begin");
		return 0;
	}

	if( GetToken::Get(in, line) != END ) {
		ParseError(line, "Missing END after statement list");
		return 0;
	}

	return new IfStatement(t.GetLinenum(), ex, stmt);
}

ParseTree *LoopStmt(istream& in, int& line) {
	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after loop");
		return 0;
	}

	Token t = GetToken::Get(in, line);

	if( t != BEGIN ) {
		ParseError(line, "Missing BEGIN after expression");
		return 0;
	}

	ParseTree *stmt = Slist(in, line);
	if( stmt == 0 ) {
		ParseError(line, "Missing statement list after BEGIN");
		return 0;
	}

	if( GetToken::Get(in, line) != END ) {
		ParseError(line, "Missing END after statement list");
		return 0;
	}

	return new LoopStatement(t.GetLinenum(), ex, stmt);
}

ParseTree *SetStmt(istream& in, int& line) {
	Token t = GetToken::Get(in, line);

	if( t != ID ) {
		ParseError(line, "Missing ID after set");
		return 0;
	}

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after id");
		return 0;
	}

	return new SetStatement(t.GetLinenum(), t.GetLexeme(), ex);
}

ParseTree *PrintStmt(istream& in, int& line) {

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after print");
		return 0;
	}

	return new PrintStatement(line, ex);
}

ParseTree *Expr(istream& in, int& line) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Token t = GetToken::Get(in, line);

		if( t != PLUS && t != MINUS ) {
			GetToken::PushBack(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new Addition(t.GetLinenum(), t1, t2);
		else
			t1 = new Subtraction(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Prod(istream& in, int& line) {
	ParseTree *t1 = Primary(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Token t = GetToken::Get(in, line);

		if( t != STAR && t != SLASH ) {
			GetToken::PushBack(t);
			return t1;
		}

		ParseTree *t2 = Primary(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == STAR )
			t1 = new Multiplication(t.GetLinenum(), t1, t2);
		else
			t1 = new Division(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Primary(istream& in, int& line) {
	Token t = GetToken::Get(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == ICONST ) {
		return new IConst(t);
	}
	else if( t == SCONST ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( GetToken::Get(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}
