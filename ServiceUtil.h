#ifndef __SERVICE_UTIL_H__
#define __SERVICE_UTIL_H__

#include "ServiceDefine.h"
#include "globe.h"
#include "LogDefine.h"

/**
 * 业务公共类，主要用于请求数据的构建
 * 应答数据的获取
 */
class ServiceUtil
{
private:
	/**
	 * 
	*/
	ServiceUtil();

	/**
	 * 
	*/
	~ServiceUtil();

public:
	/**
	 * 构造主键
	*/
    static const string makekey(int redis_type, enuServiceType serviceType, long key);

	/**
	 * 以字母开头，长度在6~16之间，只能包含字符、数字和下划线
	 */
	static bool check_password(const std::string &password);

	/**
	 * 长度在6~16之间，英文+数字
	 */
	static bool check_safepwd(const std::string &str);

	/**
	 * 生成随机数
	 */
	static uint64_t rand_number(uint64_t min, uint64_t max);

	/**
	 * 检查字符串是纯数字
	 */
	static bool check_number(const std::string &str);

	/**
 	 * @brief 计算权重
  	 * @param weights 权重值(id:weight)
 	 * @return 返回id
 	 */
	static int calculate_weight(const std::map<int, int> &weights);

	static bool check_condition(const string &src, const int num, const int condition);

	/**
	 * 替换指定字符串
	 */
	static std::string replace( const std::string &inStr, const char *pSrc, const char *pReplace );

	//格式化时间
	static std::string CurTimeFormat();

	static std::string GetTimeFormat(long time);

	static time_t GetTimeStamp(string& time);

	static int GetAgeByIDC(string& idc);

	static int GetGendeByIDC(string& idc);

	//
	static string get_country_by_ip(const string& ip);

	static long get_real_distance(const string &lat1, const string &lng1, const string &lat2, const string &lng2);

	static std::wstring s2ws(const std::string& str);

	static std::string ws2s(const std::wstring& wstr);

	static bool isEmoji(int value, int &count);

	static bool hasEmoji(const std::string& str);

	static double rad(double d);

	static double stringToDouble(const string& str);

	static vector<std::string> split(const string &str, const string &pattern);

	static std::string base64Encode(uint8_t const* src, uint32_t len);

	static std::string base64Decode(std::string const& src);

	static std::string generateUUIDStr();

	static int getWordCount(const std::string &source);

};

#endif
