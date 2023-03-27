# include <handlers/sys_info_handler.h>

void SysInfoHandler::handle(ClientConnector *responder, const WsMessage& message) {
    auto *sysInfo = new SysInfo();
    sysInfo->kernelName = "Linux";
    sysInfo->kernelVersion = "5.19.0-32-generic";
    sysInfo->arch = "x86_64";
    sysInfo->hostname = "PhucVinh";
    sysInfo->rootUser = "root";

    WsMessage res;
    res.msg = "success";
    res.error = 0;
    res.payload = sysInfo;

    responder->send(res);
}