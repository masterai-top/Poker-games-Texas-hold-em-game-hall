#ifndef __LOG_DEFINE_H__
#define __LOG_DEFINE_H__

#define LOG_VERSION "1.0"  //日志版本号
#define APP_ID 1001        //游戏App标识
#define ZONE_ID 0          //区服ID号，如果只有一个区，则填0
#define GAME_VERSION "1.0"  //游戏版本号

/***
 * 平台类型
*/
enum enuPlatformType
{
    IOS =       1,
    ANDROID =   2,
    H5 =        3,
};

/**
 * 登录方式
*/
enum enuActType
{
    LOGIN = 1,  //登入
    LOGOUT = 2, //登出
};

#endif

