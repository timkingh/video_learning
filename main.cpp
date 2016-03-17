#include <iostream>

using namespace std;

class World
{
public:
	World()
	{
		cout << "Hello!" << endl;
	}

	~World()
	{
		cout << "Good Bye!" << endl;
	}
};

World TheWorld;

int main()
{
	cout << "This is Main Function!" << endl;
    return 0;
}



