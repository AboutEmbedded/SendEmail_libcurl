#ifndef _libcurl_smtp_H
#define _libcurl_smtp_H

/* This is a simple example showing how to send mail using libcurl's SMTP
 * capabilities. For an example of using the multi interface please see
 * smtp-multi.c.
 *
 * Note that this example requires libcurl 7.20.0 or above.
 */

#include <curl/curl.h>
#include <iostream>
#include <string.h>

#include "email_config.h"

// functions
CURLcode SendEmail(Email_CFG::msg_data &to_Tx);

#endif /* _libcurl_smtp_H.h */
