#ifndef _MEDIASOURCE_H_
#define _MEDIASOURCE_H_

#include "media.h"

class MediaSource: public Medium
{
public:
    MediaSource(int id);
    ~MediaSource();

    virtual void Print();
};




#endif


