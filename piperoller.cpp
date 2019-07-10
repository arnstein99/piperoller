#include <csignal>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
using namespace std;

namespace // anonymous
{
    volatile sig_atomic_t gSignalStatus;
    unsigned int sequence = 0;

    void usage()
    {
        cerr << "Usage: logroller basename" << endl;
    }

    int init(const string& basename, unsigned int seq, ofstream& strm)
    {
        stringstream sstr;
	sstr << basename << '.' << seq;
	strm.open(sstr.str());
	if (!strm)
	{
	    cerr << "Failed to open file \"" << sstr.str() << "\" for writing"
	         << endl;
	    return 1;
	}
        return 0;
    }

    int roll(const string& basename, unsigned int& seq, ofstream& strm)
    {
        strm.close();
	return init (basename, ++seq, strm);
    }
}

extern "C" void signal_handler (int signal)
{
    gSignalStatus = signal;
}

int main (int argc, char* argv[])
{
    // Process inputs
    if (argc != 2)
    {
        usage();
	return 1;
    }
    ofstream outp;
    if (init (argv[1], sequence, outp) != 0) return 1;

    signal (SIGHUP, signal_handler);

    int chr;
    while ((chr = getchar()) != EOF)
    {
	if (chr == '\n')
	{
	    outp << endl;
	    if (gSignalStatus)
	    {
		if (roll(argv[1], sequence, outp) != 0) return 1;
		cout << "Will roll " << argv[1] << endl;
		gSignalStatus = 0;
	    }
	}
	else
	{
	    outp << static_cast<char>(chr);
	}
    }
}
