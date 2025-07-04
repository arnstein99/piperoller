#include <csignal>
#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;

namespace // anonymous
{
    volatile sig_atomic_t gSignalStatus;

    void usage()
    {
        cerr << "Usage: piperoller basename [starting_number]" << endl;
    }

    int init(const string& basename, int seq, ofstream& strm)
    {
        string filename(basename + "." + to_string(seq));
        strm.open(filename);
        if (!strm)
        {
            cerr << "Failed to open file \"" << filename << "\" for writing"
                 << endl;
            return 1;
        }
        return 0;
    }

    std::string my_time(void)
    {
        time_t tt;
        struct tm tm;
        char buf[128];
        if ((tt = time (NULL)) == -1)
        {
            perror ("time failed");
            pthread_exit (NULL);
        }
        tm = *localtime (&tt);
        snprintf (buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d",
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
        return std::string(buf);
    }

    int roll(const string& basename, int& seq, ofstream& strm)
    {
        strm << my_time() << " *** piperoller ***" << std::endl;
        strm.close();
        int retval = init (basename, ++seq, strm);
        if (retval == 0)
        {
            strm << my_time() << " *** piperoller ***" << std::endl;
        }
        return retval;
    }
} // namespace (anonymous)

extern "C" void signal_handler (int signal)
{
    gSignalStatus = signal;
}

int main (int argc, char* argv[])
{
    // Process inputs
    if ((argc != 2) && (argc != 3))
    {
        usage();
        return 1;
    }

    // Open first output file
    int sequence = 0;
    if (argc == 3)
    {
        if (sscanf (argv[2], "%d", &sequence) != 1)
        {
            cerr << "Error: illegal numeric expression \"" << argv[2] <<
                "\"" << endl;
            exit (1);
        }
        if (sequence < 0)
        {
            cerr << "Warning: sequence start will be negative" << endl;
        }
    }
    ofstream outp;
    if (init (argv[1], sequence, outp) != 0) return 1;

    // Start handling the hang-up signal
    signal (SIGHUP, signal_handler);

    // Loop over input characters
    int chr;
    while ((chr = getchar()) != EOF)
    {
        if (chr == '\n')
        {
            outp << endl;
            // Handle any signals that have come in since last line feed
            if (gSignalStatus)
            {
                if (roll(argv[1], sequence, outp) != 0) return 1;
                gSignalStatus = 0;
            }
        }
        else
        {
            outp << static_cast<char>(chr);
        }
    }
    return 0;
}
