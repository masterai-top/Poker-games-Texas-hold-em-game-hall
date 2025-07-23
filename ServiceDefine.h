#ifndef __SERVICE_DEFINE_H__
#define __SERVICE_DEFINE_H__

/***
 * 业务类型，用于区分各服务模块操作的存储和数据结构
*/
enum enuServiceType
{
    /****
     * 注册登录
    */
    LOGIN_TOKEN                 = 10, //#uid<==>token
    LOGIN_VERIFICATION_CODE     = 11, //#phone,email
    LOGIN_USER_VERIFY_CODE      = 12, //#uid<==>code

    /****
     *  用户信息
     *
     * ***/
    USER_ACCOUNT        = 20,     //#tb_useraccount,uid
    USER_INFO           = 21,     //#tb_userinfo
    USER_ADDRESS        = 22,     //#tb_user_address,uid
    USER_ADDRESS_GID    = 23,     //#tb_user_address,gid
    USER_PROPS_INFO     = 24,     //#tb_user_props
    USER_PROPS_LIST     = 25,     //#tb_user_props
    USER_REMARK         = 26,     //#tb_user_remark

    /***
     * 玩家状态
    */
    USER_STATE_ONLINE               = 31,//uid<==>online state
    USER_STATE_GAME                 = 32,//uid<==>game state
    USER_STATE_GAME_DB              = 33,// 在线状态

    /**
     * 游戏记录
    */
    GAME_RECORD_COLLECT              = 41,  // tb_game_collect, 收藏牌局，key:uid
    GAME_RECORD_DETAIL               = 42,  // tb_game_detail,
    GAME_RECORD_BRIEF                = 43,  // tb_game_brief,
    GAME_RECORD_STAT                 = 44,  // tb_game_stat,
    GAME_RECORD_CHEAT                = 45,  // tb_game_cheat,
    GAME_RECORD_OWN                  = 46,  // tb_game_own,

    /*
    * 大厅
    */
    MATCH_ROOM_MEMBER                 = 49,   //#tb_match_room_member
    MATCH_ROOM_LIST                   = 50,   //#tb_match_room_list
    ORDER_LIST                        = 51,   //#tb_order_list
    ORDER_STATS                       = 52,   //#tb_order_stats

    /**
     * 时间,用于测试
    */
    GAME_CUR_TIME = 1100,      // 时间
};

#endif
