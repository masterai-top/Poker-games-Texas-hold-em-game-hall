#include "xtime_userface.h"


/////////////////////////////////////////////////////////////////////////////
xid_t SetTimerHost(IXTimeHandler *handler, int user_cmd, xtime_t delaytime, xtime_t interval)
{
    return setTimer(handler, user_cmd, delaytime, interval);
}

xid_t SetCbTimerHost(void *user_param, std::function<int(TimerParam &)> cb, DestructorFunc_t del, int timerKey, xtime_t delaytime, xtime_t interval)
{
    return XTimer::setTimer(delaytime, cb, TimerParam(interval, timerKey, user_param, del));
}

xid_t KillTimer(xid_t xid)
{
    return killTimer(xid);
}

xtime_t GetTimerRemaining(xid_t xid)
{
    return XTimer::getTimerRemaining(xid);
}

///////////////////////////////////////////////////////////////
//handler 管理方式的实现
int IXTimeHandler::loopDo(int user_cmd, xtime_t interval)
{
    xid_t ret = setTimer(this, user_cmd, interval, interval);
    _mmap_cmd_xid.insert(std::make_pair(user_cmd, ret));
    return ret;
}

xid_t IXTimeHandler::delayDo(int user_cmd, xtime_t delaySec)
{
    xid_t ret = setTimer(this, user_cmd, delaySec, 0);
    _mmap_cmd_xid.insert(std::make_pair(user_cmd, ret));
    return ret;
}

xtime_t IXTimeHandler::remaining(int user_cmd)
{
    auto itLower = _mmap_cmd_xid.lower_bound(user_cmd);
    if (itLower == _mmap_cmd_xid.end())
        return 0;

    auto timerId = itLower->second;
    return XTimer::getTimerRemaining(timerId);
}

int IXTimeHandler::cancelDo(int user_cmd, int start, int end)
{
    auto itLower = _mmap_cmd_xid.lower_bound(user_cmd);
    auto itUpper = _mmap_cmd_xid.upper_bound(user_cmd);
    if (itLower == itUpper)
        return -1;

    int ret = 0;
    int direction = end - start;
    if (direction == 0 && start == 0)
    {
        //删除所有指定命令字定时器
        for (; itLower != itUpper;)
        {
            ret = killTimer(itLower->second);
            _mmap_cmd_xid.erase(itLower++);
        }
    }
    else if (start != 0 && direction == 0 )
    {
        //删除正向 第n新的定时器
    }
    else if ( direction > 0 )
    {
        //正向删除 最新到最早
    }
    else // direction < 0
    {
        //逆向删除 最早到最新
    }

    return ret;
}

int IXTimeHandler::clearDoAll()
{
    for (auto &kv : _mmap_cmd_xid )
    {
        killTimer(kv.second);
    }

    _mmap_cmd_xid.clear();
    return 0;
}

int IXTimeHandler::cancelDo(int cmd, xid_t xid)
{
    auto it = _mmap_cmd_xid.begin();
    auto end = _mmap_cmd_xid.end();
    while (it != end)
    {
        if (it->first == cmd && it->second == xid)
        {
            killTimer(it->second);
            _mmap_cmd_xid.erase(it++);
        }
    }

    return 0;
}

