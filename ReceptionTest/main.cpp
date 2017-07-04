#include <iostream>
#include <string>
#include <curl/curl.h>
int main() {
    CURL *con=curl_easy_init();
    curl_easy_setopt(con,CURLOPT_CONNECT_ONLY,1L);
    //curl_easy_setopt(con,CURLOPT_URL,"http://6c9fa184.ngrok.io/");
    curl_easy_setopt(con,CURLOPT_URL,"api.kik.com/v1/");
    CURLcode response=curl_easy_perform(con);
    std::cout<<response<<std::endl;
    std::string buffer;
    size_t sizeofbuffer=2048,receivedbuffer;
    std::cout<<"wait?";
    std::cin.ignore();
    while(curl_easy_recv(con,&buffer,sizeofbuffer,&receivedbuffer)!=CURLE_OK) {
        std::cout<<"Waiting...";
    } 
    std::cout<<"Received!:"<<buffer<<std::endl;
    curl_easy_cleanup(con);
}