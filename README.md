# SendEmail_libcurl

This application for Linux OS with Cmake building
you must create some steps for start

in current time we can send files (checked with files near 10mB) but!!!
you must play with waiting time (in file email_config.h parameter unsigned int max_attempt_time_)

##################################
# 1 step: install libcurl library
##################################
#how to install libcurl library
sudo apt-get install -y libcurl4-openssl-dev OR sudo ./libcurl_install.sh OR

git clone https://github.com/curl/curl.git
git checkout 6a45abfbf6c5e366b05524301e1dfeacf348be26
cd curl
autoreconf -fi
./configure --with-openssl --enable-versioned-symbols
make
sudo make install

#NOTE: if you have problem with libcurl.so.4 - execute in cmd line
#sudo rm /usr/local/lib/libcurl.so.4
#sudo ln -s /usr/lib/x86_64-linux-gnu/libcurl.so.4.7.0 /usr/local/lib/libcurl.so.4

##################################
# 2 step: create gmail.com email acccount with device password 
# (or change mailserver for yandex.ru/rambler etc)
##################################
#write your email and device password in file  email_config.h
#std::string fromEmail_ = "Your@gmail.com";
#std::string fromPass_ = "YourPass"; 


##################################
# 3 step: add TO email
##################################
#write TO email (email for receive msg) in file  email_config.h
#std::string to_ = "Receive_email";

##################################
# 4 step (optional): change msg and theme
##################################
#in file  email_config.h
#std::string subject_ = "theme";
#std::string msgText_ = "Hello from smtp";


##################################
# 5 step: build(rebuild) project
##################################
#in project directory run
./rebuild_email_sender.sh 

##################################
# 6 step: run project
##################################
#in project directory run
./start_email_sender.sh

##################################
# 7 step: check TO mailbox (maybe in spam)
##################################
#in project directory run
./start_email_sender.sh
