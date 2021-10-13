#include "kwserver.h"

#include "platform/kwserver_posix.h"

KwServer* KwServer::Create()
{
    return new KwServerPosix();
}