#include <stdio.h>
#include <curl/curl.h>
#include <string>

enum class EncryptionMethod
{
  SMTPS_SSL,
  SMTP_STL,
};

class CSendEmail
{
public:
  CSendEmail(const std::string &from_email, 
  const std::string &to_mail, 
  const std::string &cc_mail, 
  const std::string &passwd, 
  const std::string &smtp_server);

  CSendEmail(const std::string &to_mail);

  int SendMail(const std::string &subject, const std::string &content, EncryptionMethod method);

private:
  int SendSSL(void);
  int SendSTL(void);
  void SetPayloadText(const std::string &text);

private:
  std::string username_; //"form_example@126.com"
  std::string from_mail_; //"<form_example@126.com>"
  std::string to_mail_;   //"<to_example@126.com>"
  std::string cc_mail_;   //"<cc_example@126.com>"
  std::string password_;
  std::string smtp_server_;
  std::string payload_text_;
};
