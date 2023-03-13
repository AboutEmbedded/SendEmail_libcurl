#include <iostream>
#include <string>
#include "chrono" // for sending in loop
#include "thread" //

#include "libcurl_smtp.h"

#define def_TIME_SLEEP_EMAIL 1 //time between sending emails

Email_CFG::msg_data BaseCfg;


int32_t main()
{
    std::cout << "Application is started" << std::endl;
    
    int cnt = 1;
    std::thread SendEmailLoop([&](){
    while (cnt!=0) {
      std::cout << "loop..."<< cnt << std::endl;
      SendEmail(BaseCfg);
      std::this_thread::sleep_for(std::chrono::seconds(def_TIME_SLEEP_EMAIL));
      cnt--;
    } });

    std::cout << "loop still runned" << std::endl;
    SendEmailLoop.join();


    std::cout << "loop end" << std::endl;
    return 0;
}
