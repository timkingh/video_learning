#include "media.h"
#include <iostream>

using namespace std;

Medium::Medium(int id): m_identifier(id)
{
    cout << m_identifier << ":constructor of Class Medium!\n";
}

Medium::~Medium()
{
    cout << "destructor of Class Medium!\n";
}


void Medium::Print()
{
    cout << "Print of Medium!\n";
}




