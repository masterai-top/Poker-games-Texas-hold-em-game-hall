/***************************************************************************************
#  Copyright (c) 2018
#  All rights reserved
#
#  @author  :
#  @name    :
#  @file    :
#  @date    :
#  @describe:闹钟服务
#    1.需设定闹钟的主体类继Copyright 接口类,并需实现Timeout 接口
#    2.如果走秒由外部驱动,则使用getSoTickTock  获取 soTickTock 走秒函数
#    3.设计精度按秒计算,如果需要增加精度,更改外部晶振频率即可
#    4.接口类提供基本的DelayDo 和cancelDo 方法,使用需要在调用类内定义命令字，并在
#Timeout接口中实现分发。
#    5.使用delayDo时,对象
#    6.使用cancelDo时,如果定义有多次相同的命令字,可以指定顺序>>>>详见注释
#***************************************************************************************/

#include <iostream>
#include "xtimer.h"
#include <util/tc_timeprovider.h>
#include <util/tc_logger.h>
#include "servant/RemoteLogger.h"

#define IS_HANDLER_WAY  -1
#define INVALID_XID static_cast<xid_t>(-1)

namespace XTimer
{
#define ROLLLOG(level) (LOG->level() << "[" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "] ")
#define ROLLLOG_DEBUG (ROLLLOG(debug))

    /////////////////////////////////////////////////////////////////////
    int XXTime::onTickTock(int taskNum)
    {
        int dealNum = 0;
        //counter
        ++_currentTime;
        //task list
        //
        // ROLLLOG_DEBUG << "@>>>>>>>>>>>>>>>>>>>>>>>>>>>> now: "<< TNOWMS << endl;
        // ROLLLOG_DEBUG << "@bug timer before _triggerTimeID: " << _triggerTimeID.size()<< endl;

        std::map<xid_t, long> v;
        auto iter = _triggerTimeID.begin();
        auto ends = _triggerTimeID.end();
        for ( ; iter != ends; )
        {
            if(dealNum >= taskNum)
            {
                break;
            }

            if ((unsigned long)tars::TNOWMS < iter->first)
            {
                iter++;
                continue;
            }

            dealNum++;
            xid_t xid = INVALID_XID;
            auto event = _idCallbackParam.find(iter->second);
            if (event != _idCallbackParam.end())
            {
                long ts = tars::TNOWMS;
                auto &param = event->second.second;
                auto cb = event->second.first;
                xid = event->first;

                try
                {
                    if (not param._killed)
                        cb(param);
                }
                catch (...)
                {

                }

                try
                {
                    if (param._killed)
                    {
                        param._interval = 0;
                    }

                    if (param._interval == 0)
                    {
                        if (param._destructor)
                            param._destructor(&(param._userStruct));
                    }
                }
                catch (...)
                {

                }

                if ((xid != INVALID_XID) && (param._interval == 0))
                    _idCallbackParam.erase(event);

                if ((xid != INVALID_XID) && (param._interval != 0))
                {
                    _triggerTimeID.insert(std::make_pair(param._interval * 1000 + tars::TNOWMS, xid));
                    //ROLLLOG_DEBUG << "@bug inter timer  _triggerTimeID: " << _triggerTimeID.size() << ", _interval: "<< param._interval <<", timerid:"<< param.getKey() << endl;
                }

                //ROLLLOG_DEBUG << "@bug run timer  _triggerTimeID: " << _triggerTimeID.size() <<", timerid:"<< param.getKey() << endl; 
                v[param.getKey()] = tars::TNOWMS - ts;
            }
            else
            {
                //ROLLLOG_DEBUG << "@err _triggerTimeID != _idCallbackParam " << endl;
            }

            _triggerTimeID.erase(iter++);
            // ROLLLOG_DEBUG << "@bug erase timer _triggerTimeID: " << _triggerTimeID.size()<< endl;
        }

        // ROLLLOG_DEBUG << "@bug timer end _triggerTimeID: " << _triggerTimeID.size() <<", deal num: " << dealNum  << endl;
        // ROLLLOG_DEBUG << "@<<<<<<<<<<<<<<<<<<<<<<<<<<<< "<< endl;

        for (auto iter = v.begin(); iter != v.end(); iter++)
        {
            if (((*iter).first > 9999999) || ((*iter).second < 50))
                continue;

            ROLLLOG_DEBUG << "@bug timer, taskId: " << (*iter).first << ", taskCostTime:" << (*iter).second << endl;
        }

        return 0;
    }

    xtime_t XXTime::getTimerRemaining(xid_t timerId)
    {
        auto evIter = _idCallbackParam.find(timerId);
        if (evIter != _idCallbackParam.end())
        {
            TimerParam &funcParam = evIter->second.second;
            return (funcParam._triggerTime - _currentTime);
        }

        return 0;
    }

    xid_t XXTime::setTimer(xtime_t lifeTime, std::function<int(TimerParam &)> cb, TimerParam  funcParam)
    {
        // uint32_t executeTimes = lifeTime * getPeriodTimes();
        xid_t xid = generateTimerId();
        // xtime_t triggerTime = _currentTime + executeTimes;
        xtime_t triggerTime = tars::TNOWMS + lifeTime * 1000;
        funcParam._triggerTime = triggerTime;
        _triggerTimeID.insert(std::make_pair(triggerTime, xid));
        _idCallbackParam.insert(std::make_pair(xid, std::make_pair(cb, funcParam)));
        ROLLLOG_DEBUG <<"@bug timer run tome: "<< triggerTime <<", _timerIdMax: "<< _timerIdMax <<", xid=" << xid << ", _idCallbackParam: "<< _idCallbackParam.size() << ", timerid:"<< funcParam.getKey() << endl;
        return xid;
    }

    void XXTime::setPauseFlag(bool flag)
    {
        _bPause = flag;
    }

    bool XXTime::getPauseFlag()
    {
        return _bPause;
    }

    xid_t XXTime::killTimer(xid_t timerId)
    {
        auto idIter = _idCallbackParam.find(timerId);
        if (idIter != _idCallbackParam.end() )
        {
            idIter->second.second._killed = true;
        }

        return timerId;
    }

    xid_t XXTime::generateTimerId()
    {
        xid_t  xid = 0;

        do
        {
            ++_timerIdMax;
            if ( _timerIdMax >  static_cast<xid_t>(-4))
            {
                ROLLLOG_DEBUG<<"@bug timer _timerIdMax: "<< _timerIdMax<< endl;
                _timerIdMax = 0;
            }

            if (_idCallbackParam.find(_timerIdMax) == _idCallbackParam.end())
            {
                xid = _timerIdMax;
                break;
            }

            //LOG_ERROR << "Timer id is already used: timerid=" << _timerIdMax << endl;
        }
        while(true);
        return xid;
    }

    int XXTime::getPeriodTimes()
    {
        return _periodTimes;
    }

    void XXTime::setPeriodTimes(int times)
    {
        _periodTimes = times;
    }
}
