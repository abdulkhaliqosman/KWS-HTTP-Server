#include "kwserver.h"

int main()
{
    KwServer* kwServer = KwServer::Create();

    kwServer->Init();
    kwServer->Run();
}