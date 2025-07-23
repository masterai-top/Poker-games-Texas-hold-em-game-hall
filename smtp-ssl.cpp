/* <DESC>
 * SMTP example using SSL
 * </DESC>
 */
 
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <mutex>
#include <sstream>
#include "smtp-ssl.h"

#define CC_MAIL         "<jinbei01@gmx.com>"
#define FROM_MAIL       "<jinbeipoker@outlook.com>" 
#define USERNAME        "jinbeipoker@outlook.com"
#define PASSWORD        "Qzkj134%T9"
#define SMTP_SERVER     "smtp://smtp-mail.outlook.com:587"

#define SKIP_PEER_VERIFICATION //跳过证书验证
#define SKIP_HOSTNAME_VERIFICATION 

std::mutex g_mutex;

static std::string payload_text = 
  "Date: Mon, 29 Nov 2022 21:54:29 +1100\r\n"
  "To: example@example.com \r\n"
  "From: " FROM_MAIL "\r\n"
  "Cc: " CC_MAIL "\r\n"
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
  "rfcpedant.example.org>\r\n"
  "Subject: SMTP example message\r\n"
  "\r\n" /* empty line to divide headers from body, see RFC5322 */
  "The body of the message starts here.\r\n"
  "\r\n"
  "It could be a lot of lines, could be MIME encoded, whatever.\r\n"
  "Check RFC5322.\r\n";
 
struct upload_status {
  size_t bytes_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

CSendEmail::CSendEmail(const std::string &from_email, 
  const std::string &to_mail, 
  const std::string &cc_mail, 
  const std::string &passwd, 
  const std::string &smtp_server) : 
    username_(from_email),
    from_mail_("<" + from_email + ">"),
    to_mail_("<" + to_mail + ">"),
    cc_mail_("<" + cc_mail + ">"),
    password_(passwd),
    smtp_server_(smtp_server)
{
}

CSendEmail::CSendEmail(const std::string &to_mail)
  : username_(USERNAME),
    from_mail_(FROM_MAIL),
    to_mail_("<" + to_mail + ">"),
    cc_mail_(CC_MAIL),
    password_(PASSWORD),
    smtp_server_(SMTP_SERVER)
{
}

void CSendEmail::SetPayloadText(const std::string &text)
{
  g_mutex.lock();
  payload_text = text;
  //memcpy((char *)payload_text, text.c_str(), text.size());
  g_mutex.unlock();
}

int CSendEmail::SendMail(const std::string &subject, const std::string &content, EncryptionMethod method)
{
  time_t t = time(nullptr);
  auto tm = localtime(&t);
  char buffer[255] = "\0";
  strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %z", tm);

  std::string text;
  text.append("Date: ").append(buffer).append("\r\n");
  text.append("To: ").append(to_mail_).append("\r\n");
  text.append("From: ").append(from_mail_).append("\r\n");
  //text.append("Cc: ").append("\r\n");
  text.append("Subject: ").append(subject).append("\r\n");
  text.append("\r\n");
  text.append(content).append("\r\n");

  SetPayloadText(text);
  if (method == EncryptionMethod::SMTPS_SSL)
    return SendSSL();
  else if (method == EncryptionMethod::SMTP_STL)
    return SendSTL();

  return 0;
}
 
int CSendEmail::SendSSL(void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };
 
  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, username_.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
 
    /* This is the URL for your mailserver. Note the use of smtps:// rather
     * than smtp:// to request a SSL based connection. */
    curl_easy_setopt(curl, CURLOPT_URL, smtp_server_.c_str());
 
    /* If you want to connect to a site who is not using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you. */
#ifdef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
 
    /* If the site you are connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure. */
#ifdef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
 
    /* Note that this option is not strictly required, omitting it will result
     * in libcurl sending the MAIL FROM command with empty sender data. All
     * autoresponses should have an empty reverse-path, and should be directed
     * to the address in the reverse-path which triggered them. Otherwise,
     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
     * details.
     */
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_mail_.c_str());
 
    /* Add two recipients, in this particular case they correspond to the
     * To: and Cc: addressees in the header, but they could be any kind of
     * recipient. */
    recipients = curl_slist_append(recipients, to_mail_.c_str());
    //recipients = curl_slist_append(recipients, cc_mail_.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
 
    /* We are using a callback function to specify the payload (the headers and
     * body of the message). You could just use the CURLOPT_READDATA option to
     * specify a FILE pointer to read from. */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* Since the traffic will be encrypted, it is very useful to turn on debug
     * information within libcurl to see what is happening during the
     * transfer */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
 
    /* Send the message */
    res = curl_easy_perform(curl);
 
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Free the list of recipients */
    curl_slist_free_all(recipients);
 
    /* Always cleanup */
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}

int CSendEmail::SendSTL(void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };

  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, username_.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());

    /* This is the URL for your mailserver. Note the use of port 587 here,
     * instead of the normal SMTP port (25). Port 587 is commonly used for
     * secure mail submission (see RFC4403), but you should use whatever
     * matches your server configuration. */
    curl_easy_setopt(curl, CURLOPT_URL, smtp_server_.c_str());

    /* In this example, we will start with a plain text connection, and upgrade
     * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
     * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
     * will continue anyway - see the security discussion in the libcurl
     * tutorial for more details. */
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    /* If your server does not have a valid certificate, then you can disable
     * part of the Transport Layer Security protection by setting the
     * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
     *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
     *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
     * That is, in general, a bad idea. It is still better than sending your
     * authentication details in plain text though.  Instead, you should get
     * the issuer certificate (or the host certificate if the certificate is
     * self-signed) and add it to the set of certificates that are known to
     * libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
     * for more information. */
    // curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

    /* Note that this option is not strictly required, omitting it will result
     * in libcurl sending the MAIL FROM command with empty sender data. All
     * autoresponses should have an empty reverse-path, and should be directed
     * to the address in the reverse-path which triggered them. Otherwise,
     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
     * details.
     */
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_mail_.c_str());

    /* Add two recipients, in this particular case they correspond to the
     * To: and Cc: addressees in the header, but they could be any kind of
     * recipient. */
    recipients = curl_slist_append(recipients, to_mail_.c_str());
    // recipients = curl_slist_append(recipients, CC_MAIL);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    /* We are using a callback function to specify the payload (the headers and
     * body of the message). You could just use the CURLOPT_READDATA option to
     * specify a FILE pointer to read from. */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* Since the traffic will be encrypted, it is very useful to turn on debug
     * information within libcurl to see what is happening during the transfer.
     */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    /* Always cleanup */
    curl_easy_cleanup(curl);
  }

  return (int)res;
}
