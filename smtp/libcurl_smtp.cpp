#include "libcurl_smtp.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

static const char *headers_text[] = {"To: ", "From: ", "Cc: ", "Subject: ", NULL};

namespace {
struct char_for_send {
  const char *p_mailserver_ = nullptr;
  const char *p_fromEmail_ = nullptr;
  const char *p_fromPass_ = nullptr;

  const char *p_to_ = nullptr;
  const char *p_cc_ = nullptr;
} ch_for_send;

void setPreferencies(CURL *p_curl, struct curl_slist *p_recipients, Email_CFG::msg_data &to_Tx) {

  // create char* vars
  ch_for_send.p_mailserver_ = to_Tx.mailserver_.c_str();
  ch_for_send.p_fromEmail_ = to_Tx.fromEmail_.c_str();
  ch_for_send.p_fromPass_ = to_Tx.fromPass_.c_str();
  ch_for_send.p_to_ = to_Tx.to_.c_str();

  /* This is the URL for your mailserver */
  curl_easy_setopt(p_curl, CURLOPT_URL, ch_for_send.p_mailserver_);

  /*Maximum timeout in secounds*/
  curl_easy_setopt(p_curl, CURLOPT_TIMEOUT, to_Tx.max_attempt_time_);

  /* Note that this option isn't strictly required, omitting it will result
   * in libcurl sending the MAIL FROM command with empty sender data. All
   * autoresponses should have an empty reverse-path, and should be directed
   * to the address in the reverse-path which triggered them. Otherwise,
   * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
   * details.
   */
  curl_easy_setopt(p_curl, CURLOPT_MAIL_FROM, ch_for_send.p_fromEmail_);

  /* Add two recipients, in this particular case they correspond to the
   * To: and Cc: addressees in the header, but they could be any kind of
   * recipient. */
  p_recipients = curl_slist_append(p_recipients, ch_for_send.p_to_);

  for (std::string varcc : to_Tx.ccArr_) {
    p_recipients = curl_slist_append(p_recipients, varcc.c_str());
  }

  curl_easy_setopt(p_curl, CURLOPT_MAIL_RCPT, p_recipients);

  // pref for login/pass
  curl_easy_setopt(p_curl, CURLOPT_USERNAME, ch_for_send.p_fromEmail_);
  curl_easy_setopt(p_curl, CURLOPT_PASSWORD, ch_for_send.p_fromPass_);

  // using SSL
  if (to_Tx.useSSL_type_ == "CURLUSESSL_NONE") {
    curl_easy_setopt(p_curl, CURLOPT_USE_SSL, CURLUSESSL_NONE);
  } else if (to_Tx.useSSL_type_ == "CURLUSESSL_TRY") {
    curl_easy_setopt(p_curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
  } else if (to_Tx.useSSL_type_ == "CURLUSESSL_CONTROL") {
    curl_easy_setopt(p_curl, CURLOPT_USE_SSL, CURLUSESSL_CONTROL);
  } else if (to_Tx.useSSL_type_ == "CURLUSESSL_ALL") {
    curl_easy_setopt(p_curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
  } else if (to_Tx.useSSL_type_ == "CURLUSESSL_LAST") {
    curl_easy_setopt(p_curl, CURLOPT_USE_SSL, CURLUSESSL_LAST);
  }

  // curl tsl/ssl version
  if (to_Tx.ssl_version_ == "CURL_SSLVERSION_DEFAULT") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_SSLv2") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv2);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_SSLv3") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_0") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_0);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_1") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_1);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_2") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  } else if (to_Tx.ssl_version_ == "CURL_SSLVERSION_TLSv1_3") {
    curl_easy_setopt(p_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
  }

  // ignore cert
  curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYHOST, to_Tx.SSL_host_verification_);
  curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYPEER, to_Tx.SSL_peer_verification_);

  // use CA sertificate
  // note for CA sert you must registrate in with command --cacert [file]
  // curl_easy_setopt(p_curl, CURLOPT_CAINFO, cacert);
  if (to_Tx.use_CA_cert_) {
    curl_easy_setopt(p_curl, CURLOPT_CAPATH, to_Tx.path_to_CA_cert_.c_str());
  }
}
} // namespace

CURLcode SendEmail(Email_CFG::msg_data &to_Tx) {
  //to_Tx = to_Tx;
  CURL *curl;
  CURLcode res = CURLE_OK;

  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    struct curl_slist *recipients = NULL;
    struct curl_slist *slist = NULL;
    curl_mime *mime;
    curl_mime *alt;
    curl_mimepart *part;

    setPreferencies(curl, recipients, to_Tx); // apply basic preferencies

    /* Build and set the message header list. */


    //Create CC from array
    std::string ccStr;
    /*
    int arrlen = to_Tx.cc_arr_.size();
    for (int i = 0; i < arrlen; i++) {
      if (i == 0) {
        ccStr = to_Tx.cc_arr_[i];
      } else {
        ccStr += ", " + to_Tx.cc_arr_[i];
      }
    }
    */
    std::string to = "To: " + to_Tx.to_;
    std::string from = "From: " + to_Tx.fromEmail_ + "(Servers device emailer)";
    std::string cc = "Cc: " + ccStr + " copy to:";
    std::string subject = "Subject: " + to_Tx.subject_;

    headers_text[0] = to.c_str();
    headers_text[1] = from.c_str();
    headers_text[2] = cc.c_str();
    headers_text[3] = subject.c_str();

    for (const char **cpp = headers_text; *cpp; cpp++) {
      headers = curl_slist_append(headers, *cpp);
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    /* Build the mime message. */
    mime = curl_mime_init(curl);

    /* The inline part is an alternative proposing the html and the text versions of the email. */
    alt = curl_mime_init(curl);

    /* Text message. */
    std::string msgText_ = to_Tx.msgText_ + "\r\n"; //"textMsg: " only for test
    part = curl_mime_addpart(alt);
    curl_mime_data(part, msgText_.c_str(), CURL_ZERO_TERMINATED);
    curl_mime_type(part, "text/html");

    /* Create the inline part. */
    part = curl_mime_addpart(mime);
    curl_mime_subparts(part, alt);
    curl_mime_type(part, "multipart/alternative");
    slist = curl_slist_append(NULL, "Content-Disposition: inline");
    curl_mime_headers(part, slist, 1);

    /* Add the file an attachment. */
    for (std::string file_path : to_Tx.file_path_) {
      part = curl_mime_addpart(mime);
      curl_mime_filedata(part, file_path.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    /* Free lists. */
    curl_slist_free_all(recipients);
    curl_slist_free_all(headers);

    /* curl will not send the QUIT command until you call cleanup, so you
     * should be able to re-use this connection for additional messages
     * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
     * calling curl_easy_perform() again. It may not be a good idea to keep
     * the connection open for a very long time though (more than a few
     * minutes may result in the server timing out the connection), and you do
     * want to clean up in the end.
     */
    curl_easy_cleanup(curl);

    /* Free multipart message. */
    curl_mime_free(mime);
  }

  return res;
}