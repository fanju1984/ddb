#include <iostream>
#include <vector>
using namespace std;

int main() {
	int item = 10;
    int items[10];
    items[0] = 10;
    items[1] = 20;
    items[2] = 30;
    int * itemp = items;
    int ** itempp = &items;
	// int ** itempp = itemp;
	cout << "itemp :" << itemp << " " << *itemp << endl;
    cout << "itemp 1 :" << itemp + 1 << " " << *(itemp+1) << endl;
    cout << "itempp :" << itempp << " " << *itempp << " " << **itempp << endl;
    cout << "itempp 1 :" << itempp + 1 << " " << *(itempp+1) << " " << **(itempp+1) << endl;
	return 0;
}