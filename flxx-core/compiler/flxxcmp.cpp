#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "flxxcmp.hpp"
#include "flxxisa.hpp"

using namespace std;

int main(int argc, char** argv) {
    ifstream target;
    if (argc < 2) {
        cout << "Invalid arguments: correct use is ./gflx <filename> -<arg1 (opt)> -<arg2 (opt)> ..." << endl;
        return 1;
    } else {
        string target_name = argv[1];
        try
        {
            target_name += ".flxx";
            target.open(target_name);
        }
        catch(const std::exception& e)
        {
            cout << "err: file " + target_name + " not found";
            return 1;
        }
        
    }
    ofstream out;
    out.open("fout.txt");
    for (string line; target.getline(&line[0], '|');) {
        if (line[0] != '#') {
            char instruction[32] = "";
        }
    }
    target.close();
    out.close();
    return 0;
}