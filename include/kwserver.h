#ifndef KWSERVER_H
#define KWSERVER_H

class KwServer
{
public:
    static KwServer* Create();

    virtual void Init() = 0;
    virtual void Run() = 0;
};

#endif