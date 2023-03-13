#ifndef __emailer_config_H
#define __emailer_config_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

namespace Email_CFG
{
  struct msg_data
  {
    /* basic data */
    std::string mailserver_ = "smtp://smtp.gmail.com:587";
    std::string fromEmail_ = "Your_email@gmail.com"; // there you must write your email (gmail.com)
    std::string fromPass_ = "Your_device_pass";            // you must create password wor another appication and write its here
    unsigned int max_attempt_time_ = 5;                    // max time to one attempt (libcurl trying to send email <= this time)
    std::string useSSL_type_ = "CURLUSESSL_ALL";
    std::string ssl_version_ = "CURL_SSLVERSION_DEFAULT";
    bool SSL_host_verification_ = true;
    bool SSL_peer_verification_ = true;
    bool use_CA_cert_ = false;
    std::string path_to_CA_cert_ = "";

    /* data for send (changes mail to mail) */
    std::string to_ = "To"; // to email
    std::vector<std::string> ccArr_;
    std::string subject_ = "theme";
    std::string msgText_ = "Hello from smtp";
  };

}

#endif // __emailer_config_H
