#ifndef __VIRTUAL_SENSOR_SYS_INFO_HANDLER_H__
#define __VIRTUAL_SENSOR_SYS_INFO_HANDLER_H__

#include "cmd_handler.h"
#include "schemas/sys_info.h"
#include <sys/utsname.h>

class SysInfoHandler : public CmdHandler {
public:
    void handle(ClientConnector *responder, const WsMessage& message) override;
};


#endif //VIRTUAL_SENSOR_SYS_INFO_HANDLER_H
