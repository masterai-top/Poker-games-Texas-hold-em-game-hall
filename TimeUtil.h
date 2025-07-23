#pragma once
#include <string>
#include <globe.h>
#include <time.h>

namespace timeutil
{
/**
 * 时间转时间字符串
 */
inline std::string DatetimeToString(time_t time)
{
    tm *tm_ = localtime(&time);                 // 将time_t格式转换为tm结构体
    int year, month, day;                       // 定义时间的各个int临时变量。
    year = tm_->tm_year + 1900;                 // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
    month = tm_->tm_mon + 1;                    // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
    day = tm_->tm_mday;                         // 临时变量，日。

    std::string date = I2S(year) + "/" + I2S(month) + "/" + I2S(day);
    return date;                                // 返回转换日期时间后的string变量。
}

inline std::string getCurentTimeDescToString()
{
   time_t _t = time(NULL);
    char bufTime[64] = { 0 };
    struct tm* _tt;
    //localtime_s(&_tt, &_t);//系统本地时间
    _tt = gmtime(&_t);//格林时间
    _tt->tm_year += 1900;
    _tt->tm_mon += 1;
    sprintf(bufTime, "%04d-%02d-%02d %02d:%02d:%02d"
        , _tt->tm_year,_tt->tm_mon,_tt->tm_mday,_tt->tm_hour,_tt->tm_min,_tt->tm_sec);
    return std::string(bufTime);
}

inline std::string getCurentTimeDesc()
{
   time_t _t = time(NULL);
    char bufTime[64] = { 0 };
    struct tm* _tt;
    //localtime_s(&_tt, &_t);//系统本地时间
    _tt = gmtime(&_t);//格林时间
    _tt->tm_year += 1900;
    _tt->tm_mon += 1;
    sprintf(bufTime, "%04d-%02d-%02dT%02d:%02d:%02dZ"
        , _tt->tm_year,_tt->tm_mon,_tt->tm_mday,_tt->tm_hour,_tt->tm_min,_tt->tm_sec);
    return std::string(bufTime);
}

/**
 * 时间字符串转时间
 */
inline time_t strTime2unix(const std::string &timeStamp)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    sscanf(timeStamp.c_str(), "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    tm.tm_year -= 1900;
    tm.tm_mon--;
    return mktime(&tm);
}

/**
 * 
 * 通过时间戳获取星期几
 */
inline int WeekDay(time_t timestamp)
{
    tm *tm_ = localtime(&timestamp);
    return tm_->tm_wday + 1;
}

/**
* 两个时间戳是否是同一天
*/
inline bool isSameDay(time_t time1, time_t time2)
{
    return ((time1 + 28800) / 86400 == (time2 + 28800) / 86400);

    // struct tm tm1;
    // struct tm tm2;
    // localtime_r(&time1, &tm1);
    // localtime_r(&time2, &tm2);
    // return tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday;
}

/**
 * 两个时间戳是否同一个月
 */
inline bool isSameMonth(time_t time1, time_t time2)
{
    struct tm tm1;
    struct tm tm2;
    localtime_r(&time1, &tm1);
    localtime_r(&time2, &tm2);

    return tm1.tm_year == tm2.tm_year && tm1.tm_mon == tm2.tm_mon;
}

/**
 * @brief 判断两个时间是否是同一天
 * @param iDayStartTime - [in]一天的开始时间，单位是秒
 * @param iCurrentTime - [in] 要判断的时间1
 * @param iTime - [in] 要判断的时间2
 * @result bool
 */
inline bool isWithinSameTimeSegment(int iDayStartTime, time_t iCurrentTime, time_t iTime)
{
    //减掉偏移的时间后，判断是否为同一天
    iCurrentTime -= iDayStartTime;
    iTime -= iDayStartTime;

    struct tm stCurrentTm;
    struct tm stTimeTm;

    ::localtime_r(&iCurrentTime, &stCurrentTm);
    ::localtime_r(&iTime, &stTimeTm);

    return (stCurrentTm.tm_year == stTimeTm.tm_year) && (stCurrentTm.tm_yday == stTimeTm.tm_yday);
}

/**
 * 获得指定时间从某年开始的天数 
 */
#define START_YEAR (2008)
inline int timeToDay(time_t time, int fromYear = START_YEAR)
{
    struct tm tempTm;
    localtime_r(&time, &tempTm);

    //设为那年的1月1日
    tempTm.tm_year = fromYear - 1900;
    tempTm.tm_yday = 0;
    tempTm.tm_mon = 0;
    tempTm.tm_mday = 1;

    time_t yearTime = 0;
    yearTime = mktime(&tempTm);

    return (time - yearTime) / 86400;
}

/**
 * 获得某天00:00:00的时间戳
 */
inline time_t todayBeginTime(time_t now = time(nullptr))
{
    struct tm nowTm;
    localtime_r(&now, &nowTm);

    auto todayTm = nowTm;
    todayTm.tm_hour = 0;
    todayTm.tm_min = 0;
    todayTm.tm_sec = 0;
    return mktime(&todayTm);
}

/**
 * 获取某天下月第一天00:00:00的时间戳
 */
inline time_t monthNextBeginTime(time_t now = time(nullptr))
{
    struct tm nowTm;
    localtime_r(&now, &nowTm);

    auto monthTm = nowTm;
    if (monthTm.tm_mon == 11)
    {
        monthTm.tm_year += 1;
        monthTm.tm_mon = 0;;
    }
    else
    {
        monthTm.tm_mon += 1;
    }
    monthTm.tm_mday = 1;
    monthTm.tm_hour = 0;
    monthTm.tm_min = 0;
    monthTm.tm_sec = 0;
    return mktime(&monthTm);
}

/**
 * 获取某天上月第一天00:00:00的时间戳
 */
inline time_t monthBeforBeginTime(time_t now = time(nullptr))
{
    struct tm nowTm;
    localtime_r(&now, &nowTm);

    auto monthTm = nowTm;
    if (monthTm.tm_mon > 0)
    {
        monthTm.tm_mon -= 1;
    }
    else
    {
        monthTm.tm_year -= 1;
        monthTm.tm_mon = 11;
    }
    monthTm.tm_mday = 1;
    monthTm.tm_hour = 0;
    monthTm.tm_min = 0;
    monthTm.tm_sec = 0;
    return mktime(&monthTm);
}

/**
 * 获取某天当月第一天00:00:00的时间戳
 */
inline time_t monthBeginTime(time_t now = time(nullptr))
{
    struct tm nowTm;
    localtime_r(&now, &nowTm);

    auto monthTm = nowTm;
    monthTm.tm_mday = 1;
    monthTm.tm_hour = 0;
    monthTm.tm_min = 0;
    monthTm.tm_sec = 0;
    return mktime(&monthTm);
}

/**
 * 是否超过了n天 
 */
inline bool expireNDays(time_t time, int64_t n, time_t now = ::time(nullptr))
{
    return timeToDay(time) + n <= timeToDay(now);
}

/**
 * 两个时间戳相差多少个自然天
 */
inline int DifferenceDays(time_t start, time_t end)
{
    if (end < start)
        return -1;

    return timeToDay(end) - timeToDay(start);
}

/**
 *  lastUpdateTime到now直接是否跨过了星期n(0-6)
 */
inline bool expireOneWeek(time_t lastUpdateTime, int64_t n, time_t now = ::time(nullptr))
{
    struct tm lastTm;
    ::localtime_r(&lastUpdateTime, &lastTm);

    //计算距离上次更新后最近的刷新时间
    auto offsetDay = ((n + 7) - lastTm.tm_wday) % 7;  //差几天
    if (0 == offsetDay)
    {
        //lastupdatetime当天就是指定的星期n, 已经刷新过了，认为下一个刷新日要在一周以后，所以返回false
        return false;
    }

    auto nextResetDayTime = lastUpdateTime + offsetDay * 86400;
    struct tm nextResetDayTm;
    ::localtime_r(&nextResetDayTime, &nextResetDayTm);

    //获得0点的时间戳
    nextResetDayTm.tm_sec = 0;
    nextResetDayTm.tm_min = 0;
    nextResetDayTm.tm_hour = 0;
    nextResetDayTime = mktime(&nextResetDayTm);

    return timeToDay(nextResetDayTime) <= timeToDay(now);
}

inline int64_t GetRoleID(uint16_t gameID, int64_t uin, uint16_t roleIDSeq)
{
    uint64_t roleID = 0;
    //低32位和uin相同
    roleID = (((uint64_t)uin) & 0xffffffff);
    //16位gameid
    roleID |= (((uint64_t)gameID) << 32);
    //1
    roleID |= 0x1000000000000;
    //14位自增id
    roleID |= (((uint64_t)roleIDSeq) << 49);
    return (int64_t)roleID;
}

inline int randomNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// 占位符替换,类似Python
// Usage: auto t = formatString("INSERT INTO {0} WHERE `id` = {1}, `value` = {0}, `name` = {2}", toStringVec(111, 999, "wzc"));
inline std::string formatString(std::string target, const std::vector<std::string> &subs)
{
    for (size_t i = 0; i < subs.size(); ++i)
    {
        std::string placeholders = "{" + std::to_string(i) + "}";
        const std::string &sub = subs.at(i);
        size_t pos = target.find(placeholders);
        size_t placeholders_len = placeholders.length();
        while (pos != std::string::npos)
        {
            target.replace(pos, placeholders_len, sub);
            pos = target.find(placeholders);
        }
    }

    return target;
}

template <typename T>
std::vector<std::string> &toStringVec(std::vector<std::string> &subsVec, T t)
{
    ostringstream os;
    os << t;
    subsVec.push_back(os.str());
    return subsVec;
}

template <typename T, typename... Args>
std::vector<std::string> toStringVec(std::vector<std::string> &subsVec, T head, Args... args)
{
    ostringstream os;
    os << head;
    subsVec.push_back(os.str());
    return toStringVec(subsVec, args...);
}

} //namespace timeutil