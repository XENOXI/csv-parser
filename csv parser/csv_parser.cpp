#include <iostream>
#include <tuple>
#include "csv.h"

using namespace std;
int main()
{
    ifstream file("file.csv");

    CSVParser<int,double, string> parser(file, 0 /*skip first lines count*/);
    for (tuple<int,double, string> rs : parser) {
        cout << rs << endl;
    }
    return 0;
}
