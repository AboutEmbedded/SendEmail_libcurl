#include "libcurl_smtp.h"

static const char *payload_text[100] = {"To: \r\n",
                                        "From: \r\n",
                                        "Cc: \r\n",
                                        "Subject: \r\n",
                                        "\r\n", /* empty line to divide headers from body, see RFC5322 */

                                        "Message text start here\r\n",
                                        NULL};

struct upload_status
{
  int lines_read;
};

namespace
{
  struct char_for_send
  {
    const char *p_mailserver_ = nullptr;
    const char *p_fromEmail_ = nullptr;
    const char *p_fromPass_ = nullptr;

    const char *p_to_ = nullptr;
    const char *p_cc_ = nullptr;
    // const char* subject_=nullptr;
    // const char* msgText_=nullptr;

  } ch_for_send;
} // namespace

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
  {
    return 0;
  }

  data = payload_text[upload_ctx->lines_read];

  if (data)
  {
    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;

    return len;
  }

  return 0;
}

void setPreferencies(CURL *p_cupl, struct curl_slist *p_recipients, struct upload_status *p_upload_ctx,
                     Email_CFG::msg_data &to_Tx)
{

  // create char* vars
  ch_for_send.p_mailserver_ = to_Tx.mailserver_.c_str();
  ch_for_send.p_fromEmail_ = to_Tx.fromEmail_.c_str();
  ch_for_send.p_fromPass_ = to_Tx.fromPass_.c_str();
  ch_for_send.p_to_ = to_Tx.to_.c_str();

  /* This is the URL for your mailserver */
  curl_easy_setopt(p_cupl, CURLOPT_URL, ch_for_send.p_mailserver_);

  /*Maximum timeout in secounds*/
  curl_easy_setopt(p_cupl, CURLOPT_TIMEOUT, to_Tx.max_attempt_time_);

  /* Note that this option isn't strictly required, omitting it will result
   * in libcurl sending the MAIL FROM command with empty sender data. All
   * autoresponses should have an empty reverse-path, and should be directed
   * to the address in the reverse-path which triggered them. Otherwise,
   * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
   * details.
   */
  curl_easy_setopt(p_cupl, CURLOPT_MAIL_FROM, ch_for_send.p_fromEmail_);

  /* Add two recipients, in this particular case they correspond to the
   * To: and Cc: addressees in the header, but they could be any kind of
   * recipient. */
  p_recipients = curl_slist_append(p_recipients, ch_for_send.p_to_);

  for (std::string varcc : to_Tx.ccArr_)
  {
    p_recipients = curl_slist_append(p_recipients, varcc.c_str());
  }

  curl_easy_setopt(p_cupl, CURLOPT_MAIL_RCPT, p_recipients);

  /* We're using a callback function to specify the payload (the headers and
   * body of the message). You could just use the CURLOPT_READDATA option to
   * specify a FILE pointer to read from. */
  curl_easy_setopt(p_cupl, CURLOPT_READFUNCTION, payload_source);
  curl_easy_setopt(p_cupl, CURLOPT_READDATA, p_upload_ctx);
  curl_easy_setopt(p_cupl, CURLOPT_UPLOAD, 1L);

  // pref for login/pass
  curl_easy_setopt(p_cupl, CURLOPT_USERNAME, ch_for_send.p_fromEmail_);
  curl_easy_setopt(p_cupl, CURLOPT_PASSWORD, ch_for_send.p_fromPass_);

  // using SSL
  curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

  // using SSL
  if (to_Tx.useSSL_type_ == "CURLUSESSL_NONE")
  {
    curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_NONE);
  }
  else if (to_Tx.useSSL_type_ == "CURLUSESSL_TRY")
  {
    curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
  }
  else if (to_Tx.useSSL_type_ == "CURLUSESSL_CONTROL")
  {
    curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_CONTROL);
  }
  else if (to_Tx.useSSL_type_ == "CURLUSESSL_ALL")
  {
    curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
  }
  else if (to_Tx.useSSL_type_ == "CURLUSESSL_LAST")
  {
    curl_easy_setopt(p_cupl, CURLOPT_USE_SSL, CURLUSESSL_LAST);
  }

  // curl tsl/ssl version
  if (to_Tx.ssl_version_ == "CURL_SSLVERSION_DEFAULT")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_SSLv2")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv2);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_SSLv3")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_0")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_0);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_1")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_1);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_2")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  }
  else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_3")
  {
    curl_easy_setopt(p_cupl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
  }

  // ignore cert
  curl_easy_setopt(p_cupl, CURLOPT_SSL_VERIFYHOST, to_Tx.SSL_host_verification_);
  curl_easy_setopt(p_cupl, CURLOPT_SSL_VERIFYPEER, to_Tx.SSL_host_verification_);

  // use CA sertificate
  // note for CA sert you must registrate in with command --cacert [file]
  // curl_easy_setopt(p_cupl, CURLOPT_CAINFO, cacert);
  if (to_Tx.use_CA_cert_)
  {
    curl_easy_setopt(p_cupl, CURLOPT_CAPATH, to_Tx.path_to_CA_cert_.c_str());
  }
}

CURLcode SendEmail(Email_CFG::msg_data &to_Tx)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx;

  upload_ctx.lines_read = 0;

  curl = curl_easy_init(); //
  if (curl)
  {
    setPreferencies(curl, recipients, &upload_ctx, to_Tx); // apply basic preferencies

    // str to email, need add NULL at the end
    std::string str_buf;

    // sreate header msg
    std::string to = "To: " + to_Tx.to_ + "\r\n";
    std::string from = "From: " + to_Tx.fromEmail_ + "(Servers device emailer)\r\n";

    std::string ccStr;
    int arrlen = to_Tx.ccArr_.size();
    for (int i = 0; i < arrlen; i++)
    {
      if (i == 0)
      {
        ccStr = to_Tx.ccArr_[i];
      }
      else
      {
        ccStr += ", " + to_Tx.ccArr_[i];
      }
    }

    std::string Cc = "Cc: " + ccStr + " copy to:\r\n";
    std::string subject_ = "Subject: " + to_Tx.subject_ + "\r\n";
    std::string msgText_ = "textMsg: " + to_Tx.msgText_ + "\r\n"; //"textMsg: " only for test

    // msg_data.update_data_ptr();

    payload_text[0] = to.c_str();
    payload_text[1] = from.c_str();
    payload_text[2] = Cc.c_str();
    payload_text[3] = subject_.c_str();
    // payload_text[4]="\r\n"; //see RFC5322
    payload_text[5] = msgText_.c_str();
    payload_text[6] = NULL;

    //-----------
    /* Send the message */
    res = curl_easy_perform(curl);

    // msg_data.clearMsgData(); // delete all data for msg which we send

    /* Check for errors */
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    /* curl won't send the QUIT command until you call cleanup, so you should
     * be able to re-use this connection for additional messages (setting
     * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
     * curl_easy_perform() again. It may not be a good idea to keep the
     * connection open for a very long time though (more than a few minutes
     * may result in the server timing out the connection), and you do want to
     * clean up in the end.
     */
    curl_easy_cleanup(curl);
  }
  return res;
}
