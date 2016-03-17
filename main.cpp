#include <iostream>

using namespace std;


class Matter
{
public:
	Matter(int idx):m_id(idx)
	{
		cout << m_id << ":" << "This is a matter!\n";
	}

	~Matter()
	{
		cout << m_id << ":" << "matter destructor!\n";
	}

private:
	const int m_id;
};


class World
{
public:
	World(int idx):m_Matter(idx),m_identifier(idx)
	{
		cout << m_identifier << ":" << "Hello!" << endl;
	}

	~World()
	{
		cout << m_identifier << ":" << "Good Bye!" << endl;
	}

	Matter m_Matter;

private:
	const int m_identifier;	
};

World TheWorld(1);

int main()
{	
	World smallWorld(2);
	cout << "This is Main Function!" << endl;

	for(int i = 3; i < 5;i++)
	{
		//World aWorld(i);
	}
    return 0;
}



