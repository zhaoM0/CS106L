#include "hashmap.h"
#include <set>
using namespace std;
#include <iostream>

void init_map(HashMap<string, int>& map);

int main() {
    HashMap<string, int> imap;
    init_map(imap);

    HashMap<string, int> cmap(imap);

    for(auto& [key, val] : cmap) {
        cout << "(" << key << "," << val << ")" << endl;
    }
    return 0;
}

void init_map(HashMap<string, int>& map) {
    cout << "Hello from your past and current lecturers!" << endl;
    map.insert({"Anna", 2019});
    map.insert({"Avery", 2019});
    map.insert({"Nikhil", 2020});
    map.insert({"Ethan", 2020});
    map.insert({"Frankie", 2021});
    map.insert({"Sathya", 2021});
    //demonstrate the map in action!
    map.debug();
    map.rehash(5);
    map.debug();
}