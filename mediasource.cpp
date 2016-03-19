#include "mediasource.h"
#include <iostream>

using namespace std;

MediaSource::MediaSource(int id): Medium(id)
{
	cout << m_identifier << ":constructor of Class MediaSource!\n";
}


MediaSource::~MediaSource()
{
	cout << "destructor of Class MediaSource!\n";
}


void MediaSource::Print()
{
	cout << "Print of MediaSource!\n";
}









