#include <handlers/sys_info_handler.h>
#include <sys/utsname.h>

void SysInfoHandler::handle(ClientConnector *responder, const WsMessage& message) {
    struct utsname systemName;
    uname(&systemName);

    auto *sysInfo = new SysInfo();
    sysInfo->kernelName = systemName.sysname;
    sysInfo->kernelVersion = systemName.release;
    sysInfo->arch = systemName.machine;
    sysInfo->hostname = systemName.domainname;
    sysInfo->rootUser = "root";

    WsMessage res = WsMessage::from(message);
    res.msg = "success";
    res.error = 0;
    res.payload = sysInfo;

    responder->send(res);
}