#ifndef _MEDIA_H_
#define _MEDIA_H_


class Medium
{
public:
    Medium(int id);
    virtual ~Medium();

    virtual void Print();

protected:
    int m_identifier;
};








#endif
