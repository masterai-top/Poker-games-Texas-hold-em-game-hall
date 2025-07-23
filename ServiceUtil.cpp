#include "ServiceUtil.h"
#include "globe.h"
#include "uuid.h"
#include <regex>
#include <random>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "GeoLite2/GeoLite2PP.h"

#define pi 3.1415926535897932384626433832795
#define EARTH_RADIUS 6371.393 //地球半径 KM

/**
 * 
*/
ServiceUtil::ServiceUtil()
{

}

/**
 * 
*/
ServiceUtil::~ServiceUtil()
{
    
}

/**
 * 构造主键
*/
const string ServiceUtil::makekey(int redis_type, enuServiceType serviceType, long key)
{
    return "";
}

/**
 * 以字母开头，长度在8~16之间，开头 字母
 */
bool ServiceUtil::check_password(const std::string &str)
{
    try
    {
        if (str.empty())
            return false;

        if(str.size() < 8 || str.size() > 16)
        {
            return false;
        }

        int startChr = (int)str[0];
        if((startChr < 65 || startChr > 90) && (startChr < 97 || startChr> 122))
        {
            return false;
        }

        bool hasNumber = false;
        for (unsigned int i = 1; i < str.size(); i++)
        {
            int curChr = (int)str[i];
            if (curChr >= 48 && curChr <= 57)//数字
            {
                hasNumber = true;
            }
        }
        return hasNumber;
    }
    catch(const std::exception& e)
    {
       LOG_ERROR << e.what() << endl;
    }
    return false;    
}

/**
 * 长度6位纯数字
 */
bool ServiceUtil::check_safepwd(const std::string &str)
{
    try
    {
        if (str.empty())
            return false;

        if(str.size() != 6)
        {
            return false;
        }


        bool hasNumber = false;
        for (unsigned int i = 0; i < str.size(); i++)
        {
            int curChr = (int)str[i];
            if (curChr >= 48 && curChr <= 57)//数字
            {
                hasNumber = true;
            }
            else
            {
                return false;
            }
        }
        return hasNumber;
    }
    catch(const std::exception& e)
    {
       LOG_ERROR << e.what() << endl;
    }
    return false;
}


/**
 * 检查字符串是纯数字
 */
bool ServiceUtil::check_number(const std::string &str)
{
    try
    {
        if (str.empty())
            return false;

        for (unsigned int i = 0; i < str.size(); i++)
        {
            if ((int)str[i] >= 48 && (int)str[i] <= 57)
            {
                continue;
            }
            else
            {
                return false;
            }
        }

    }
    catch(const std::exception& e)
    {
       LOG_ERROR << e.what() << endl;
       return false;
    }
    return true;
}

/**
 * 生成随机数
 */
uint64_t ServiceUtil::rand_number(uint64_t min, uint64_t max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

/**
 * @brief 计算权重
 * @param weights 权重值(id:weight)
 * @return 返回id
 */
int ServiceUtil::calculate_weight(const std::map<int, int> &weights)
{
    int sumWeight = 0;
    for (auto &it : weights)
    {
        sumWeight += it.second;
    }

    int raio = rand_number(0, sumWeight);
    for (auto &it : weights)
    {
        raio -= it.second;
        if (raio <= 0)
            return it.first;
    }
    return 0;
}

/**
 * @brief 检查字符串是否满足条件
 * @param src 字符串  num 满足条件的个数  condition 1:连续递增 0:连续相等 -1:连续递减
 * @return 
 */
bool ServiceUtil::check_condition(const string &src, const int num, const int condition)
{
    unsigned int result_bit_num = 0;
    for(unsigned int i = 1; i < src.length(); i++)
    {
        if(src[i] - src[i-1] == condition)
        {
            result_bit_num += pow(2 , i);
        }
    }

    int condition_count = 1;
    while(result_bit_num != 0)
    {
        result_bit_num &= (result_bit_num<<1);
        condition_count+=1;
    }
    return condition_count >= num;
}

std::string ServiceUtil::replace( const std::string &inStr, const char *pSrc, const char *pReplace )
{
    std::string str = inStr;
    std::string::size_type stStart = 0;
    std::string::iterator iter = str.begin();
    while (iter != str.end())
    {
        // 从指定位置 查找下一个要替换的字符串的起始位置。
        std::string::size_type st = str.find( pSrc, stStart );
        if ( st == str.npos )
            break;

        iter = iter + st - stStart;
        // 将目标字符串全部替换。
        str.replace( iter, iter + strlen( pSrc ), pReplace );
        iter = iter + strlen( pReplace );
        // 替换的字符串下一个字符的位置
        stStart = st + strlen( pReplace );
    }

    return str;
}

//格式化时间
std::string ServiceUtil::CurTimeFormat()
{
    std::string sFormat("%Y-%m-%d %H:%M:%S");
    time_t t = TNOW;
    auto ptr = localtime(&t);
    if (!ptr)
        return string("");

    char buffer[255] = "\0";
    strftime(buffer, sizeof(buffer), sFormat.c_str(), ptr);
    return string(buffer);
}

//格式化自定义时间
std::string ServiceUtil::GetTimeFormat(long time)
{
    string sFormat("%Y-%m-%d %H:%M:%S");
    time_t t = time_t(time);
    auto pTm = localtime(&t);
    if (pTm == NULL)
        return "";

    char sTimeString[255] = "\0";
    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), pTm);
    return string(sTimeString);
}

//转换时间戳
time_t ServiceUtil::GetTimeStamp(string& time)
{
    struct tm timeinfo;
    strptime(time.c_str(), "%Y-%m-%d %H:%M:%S",  &timeinfo);
    time_t timeStamp = mktime(&timeinfo);
    return timeStamp;
}

int ServiceUtil::GetAgeByIDC(string& idc)
{
    if(idc.size() < 15)
    {
        return 0;
    }
    string subIdcNo = idc.substr(6, 8);
    string sBirthDay = subIdcNo.substr(0, 4) + "-" + subIdcNo.substr(4, 2) + "-" + subIdcNo.substr(6, 2) + " 00:00:00";
    return (TNOW - GetTimeStamp(sBirthDay)) / (365 * 24 * 3600);
}

int ServiceUtil::GetGendeByIDC(string& idc)
{
    if(idc.size() < 15)
    {
        return 1;
    }
    int index = idc.size() == 15 ? 14 : 16;
    string subIdcNo = idc.substr(index, 1);
    return S2I(subIdcNo) % 2 == 0 ? 2 : 1;
}

string ServiceUtil::get_country_by_ip(const string& ip) 
{
  std::string ipdb_path("/data/app/ip/GeoLite2-Country.mmdb");

  std::shared_ptr<GeoLite2PP::DB> db;
  db = std::make_shared<GeoLite2PP::DB>(ipdb_path);

  string country;
  db->get_geoinfo(ip.c_str(), country);
  return country;
}

double ServiceUtil::rad(double d)
{
    return d * pi /180.0;
}

double ServiceUtil::stringToDouble(const string& str)
{
    istringstream iss(str);
    double num;
    iss >> num;
    return num;
}

//lat1第一个点纬度,lng1第一个点经度,lat2第二个点纬度,lng2第二个点经度
long ServiceUtil::get_real_distance(const string &lat1, const string &lng1, const string &lat2, const string &lng2)
{

    double a;
    double b;
    double radLat1 = rad(stringToDouble(lat1));
    double radLat2 = rad(stringToDouble(lat2));
    a = radLat1 - radLat2;
    b = rad(stringToDouble(lng1)) - rad(stringToDouble(lng2));
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = s * 1000;
    return std::ceil(s);
}

std::wstring ServiceUtil::s2ws(const std::string& str) {
  if (str.empty()) {
    return L"";
  }
  unsigned len = str.size() + 1;
  setlocale(LC_CTYPE, "en_US.UTF-8");
  std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
  mbstowcs(p.get(), str.c_str(), len);
  std::wstring w_str(p.get());
  return w_str;
}

std::string ServiceUtil::ws2s(const std::wstring& w_str) {
    if (w_str.empty()) {
      return "";
    }
    unsigned len = w_str.size() * 4 + 1;
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<char[]> p(new char[len]);
    wcstombs(p.get(), w_str.c_str(), len);
    std::string str(p.get());
    return str;
}

bool ServiceUtil::isEmoji(int value, int &count)
{
    //https://www.w3school.com.cn/charsets/ref_emoji.asp
    if ((value >= 0xd800 && value <= 0xdbff))
    {
        count = 2;
        return true;
    }
    else if ((0x2100 <= value && value <= 0x27ff && value != 0x263b)
        || (0x2b05 <= value && value <= 0x2b07)
        || (0x2934 <= value && value <= 0x2935)
        || (0x3297 <= value && value <= 0x3299)
        || (0x1f004 <= value && value <= 0x1f9e6)
        || value == 0xa9 || value == 0xae || value == 0x303d || value == 0x3030
        || value == 0x2b55 || value == 0x2b1c || value == 0x2b1b || value == 0x2b50
        || value == 0x231a)
    {
        count = 1;
        return true;
    }
    return false;
}

bool ServiceUtil::hasEmoji(const std::string& str)
{
    wstring content = s2ws(str);
    for(unsigned int i = 0; i < content.length(); i++)
    {
        int eCount = 0;
        if(isEmoji(content.at(i), eCount))
        {
            return true;
        }
    }
    return false;
}

vector<std::string> ServiceUtil::split(const string &str, const string &pattern)
{
    return TC_Common::sepstr<string>(str, pattern);
}

static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool isBase64(uint8_t c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string ServiceUtil::base64Encode(uint8_t const* src, uint32_t len) {

    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char charArray3[3] = {0};
    unsigned char charArray4[4] = {0};

    while (len--) {
        charArray3[i++] = *(src++);
        if (i == 3) {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; (i < 4); i++) {
                ret += base64Chars[charArray4[i]];
            }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++) {
            charArray3[j] = '\0';
        }

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++) {
            ret += base64Chars[charArray4[j]];
        }

        while ((i++ < 3)) {
            ret += '=';
        }
    }

    return ret;
}

std::string ServiceUtil::base64Decode(std::string const& src) {
    int32_t srcPosition = src.size();
    uint8_t charArray4[4] = {0};
    uint8_t charArray3[3] = {0};
    std::string ret;

    int32_t i = 0;
    int32_t j = 0;
    int32_t srcIndex = 0;
    while (srcPosition-- && (src[srcIndex] != '=') && isBase64(src[srcIndex])) {
        charArray4[i++] = src[srcIndex]; srcIndex++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                charArray4[i] = base64Chars.find(charArray4[i]);
            }

            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
            charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

            for (i = 0; i < 3; i++) {
                ret += charArray3[i];
            }

            i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++) {
            charArray4[j] = base64Chars.find(charArray4[j]);
        }

        charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
        charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);

        for (j = 0; j < i - 1; j++) {
            ret += charArray3[j];
        }
    }

    return ret;
}

std::string ServiceUtil::generateUUIDStr()
{
    uuid_t uuid;
    uuid_generate(uuid);

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    uuid_unparse(uuid, buf);

    string strRet;
    strRet.assign(buf, strlen(buf));
    return strRet;
}

//获取字符串的字数
int ServiceUtil::getWordCount(const std::string &source)
{
    int count = 0;
    int i = 0;
    while (source[i] != 0)
    {
        if (source[i] & 0x80 && source[i] & 0x40 && source[i] & 0x20)
        {
            int byteCount = 0;
            if (source[i] & 0x10)
            {
                byteCount = 4;
            }
            else
            {
                byteCount = 3;
            }
            for (int a = 0; a < byteCount; a++)
            {
                //sourceWithOutSymbol += source[i];
                i++;
            }
        }
        else if (source[i] & 0x80 && source[i] & 0x40)
        {
            i += 2;
        }
        else
        {
            i += 1;
        }
        count++;
    }
    return count;
}
