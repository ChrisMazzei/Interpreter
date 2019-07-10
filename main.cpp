
/*
 * main.cpp
 */

#include "tokens.h"
#include "parse.h"
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;


void RunTimeError(string str)
{
    cout << "RUNTIME ERROR " << str << endl;
    exit(1);
}

int
main(int argc, char *argv[])
{
	ifstream file;
	istream *in;
	int linenum = 0;

	if( argc == 1 ) {
		in = &cin;
	}

	else if( argc == 2 ) {
		file.open(argv[1]);
		if( file.is_open() == false ) {
			cout << "COULD NOT OPEN " << argv[1] << endl;
			return 1;
		}
		in = &file;
	}

	else {
		cout << "TOO MANY FILENAMES" << endl;
		return 1;
	}

	ParseTree *prog = Prog(*in, linenum);

	if( prog == 0 )
		return 0;
    
    prog->Eval();
    return 0;

	auto nCount = prog->NodeCount();
	if( nCount )
		cout << "NODE COUNT: " << nCount << endl;
	auto lCount = prog->LeafCount();
	auto iCount = nCount - lCount;
	if( iCount )
		cout << "INTERIOR COUNT: " << iCount << endl;
	auto opCount = prog->OpCount();
	if( opCount )
		cout << "OPS COUNT: " << opCount << endl;
	auto sCount = prog->StringCount();
	if( sCount )
		cout << "STRING COUNT: " << sCount << endl;
	auto md = prog->MaxDepth();
	cout << "MAX DEPTH: " << md << endl;

	return 0;
}
