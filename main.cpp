#include <iostream>
#include <string>
#include <json.hpp>
#include "hmac.h"
#include "sha1.h"
#include <curl/curl.h>
#include <cppcodec/base32_default_crockford.hpp>
#include <cppcodec/base64_default_rfc4648.hpp>
CURLcode getBearerToken(std::string* keyvar);
CURLcode getOAuth(std::string* oTokenVar);
CURLcode getCurrentTrend(std::string* trendvar);
CURLcode getTrendTweet(std::string trend, std::string* tweetvar);
CURLcode postTrendTweet(std::string trend);
std::string hexStringtoASCII(std::string hexString);
std::string getRandomString();
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
std::time_t time0=NULL,time1=NULL; // Time1 - TimeNought = time in between

struct authencation {
    std::string user="PostPirateBot";
    std::string pass="3494279";
    std::string consumerKey="DADw7W9majB5gnDuSCsFFeyPL";
    std::string consumerSecret="fWwkjSaHEeCdvwOYZVUlIjO9uoVHrGR3CBbRBVzhFCt8BowUut";
    //std::string consumerSecret="kAcSOqF21Fu85e7zjz7ZN2U4ZRhfV3WpwPAoE3Z7kBw";
    std::string accessToken="869274271155011585-pRNc6d0NWjQWe5091qFBc3ksM00KVM3";
    std::string accessSecret="7XdKp0PCLr5oXqHnBSLeY26nAcYpeVbTTPNnfDAj1GDB1";
    //std::string accessSecret="LswwdoUaIvS8ltyTt5jkRh4J50vUPVVHtR2YPi5kE";
    std::string encodedAuth= consumerKey+":"+consumerSecret; //Base64 encoded consumerKey:consumerSecret
};
std::string accessKey="",oAuthSecret="",bearerToken="";
std::string responseData="",oAuthToken="",oAuthAccessKey="";
int main() {
    short stop1,stop2;
    std::cout<<"Hi";
    if(true) { //Ignore
        std::cout<<"Hello\n";
        stop1=getBearerToken(&accessKey);
        //stop2=getOAuth(&oAuthToken);
        std::cout<<"\nStarting loop\n";
        while(!stop1) { //Come back and add stop2 for fun (get OAuth) which is now added in accessToken/accessSecret
            if((std::time(NULL) - time0)>=(150000)) {//Get current time and check if 15 minutes have passed since last time
                std::string currentTrend="",currentTrendTweet="";
                if(!getCurrentTrend(&currentTrend)) {
                    std::cout<<currentTrend<<":Trend\n";
                    if(!getTrendTweet(currentTrend,&currentTrendTweet)) {
                        std::cout<<currentTrendTweet<<":Trend Tweet\n";
                    }
                }
                time0 = std::time(NULL); //Reset timer
            }
        }
    }
    std::cout<<"End result: "<<stop1<<std::endl;
} 

CURLcode getBearerToken(std::string* keyvar) {
    CURL* active=curl_easy_init();
    struct curl_slist *headers=NULL;
    authencation credentials;
    std::string response;
    curl_slist_append(headers,"Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
    //curl_slist_append(headers,"Authorization: Basic d1dUaUQ0eVF2cVBXemcyNGdsVFdJcjRzOTp3QXlwTTlERXMzTjhWZzBOazZwdURtTEdsMjd2akpGTGEwVnNvVGp3czVxYThVZDV2YQ==");
    //curl_slist_append(headers,"Content-Length: 29");
    //curl_slist_append(headers,"Accept-Encoding: gzip");
    curl_easy_setopt(active,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(active,CURLOPT_URL,"https://api.twitter.com/oauth2/token");
    curl_easy_setopt(active,CURLOPT_USERAGENT,"PostPirate v1.0");
    curl_easy_setopt(active,CURLOPT_HTTPAUTH,CURLAUTH_BASIC);
    curl_easy_setopt(active,CURLOPT_USERPWD,credentials.encodedAuth.c_str());
    curl_easy_setopt(active,CURLOPT_POSTFIELDS,"grant_type=client_credentials");
    curl_easy_setopt(active,CURLOPT_WRITEFUNCTION,write_callback);
    curl_easy_setopt(active,CURLOPT_VERBOSE,0);
    responseData.clear();
    CURLcode response_code=curl_easy_perform(active);
    if(!responseData.empty()) {
        bearerToken=nlohmann::json::parse(responseData)["access_token"];
        std::cout<<bearerToken<<"<-Token\n";
    }
    curl_easy_cleanup(active);
    return response_code;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    //userdata=ptr;
    responseData+=ptr;
    std::cout<<responseData<<std::endl;
    std::cout<<size*nmemb<<std::endl;
    return size*nmemb;
}

CURLcode getCurrentTrend(std::string* trendvar) {
    struct curl_slist *tokenheader=NULL;
    CURL* active=curl_easy_init();
    std::string tokenheader_string="Authorization: Bearer "+bearerToken;
    tokenheader=curl_slist_append(tokenheader,tokenheader_string.c_str());
    tokenheader=curl_slist_append(tokenheader,"Accept: application/json");
    tokenheader=curl_slist_append(tokenheader,"Accept-Charset: utf-8");
    tokenheader=curl_slist_append(tokenheader,"Accept-Encoding: identity");
    curl_easy_setopt(active,CURLOPT_HTTPGET,1L);
    curl_easy_setopt(active,CURLOPT_URL,"https://api.twitter.com/1.1/trends/place.json?id=1");
    curl_easy_setopt(active,CURLOPT_USERAGENT,"PostPirate v1.0");
    curl_easy_setopt(active,CURLOPT_WRITEFUNCTION,write_callback);
    curl_easy_setopt(active,CURLOPT_VERBOSE,0);
    curl_easy_setopt(active,CURLOPT_HTTPHEADER,tokenheader);
    responseData.clear();
    CURLcode response_code=curl_easy_perform(active);
    if(!responseData.empty()&&response_code==0) {
        *trendvar=nlohmann::json::parse(responseData.c_str())[0]["trends"][0]["name"];
    }
    curl_easy_cleanup(active);
    return response_code;
}

std::string getRandomString() {
    std::string alphas= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string returnVal="";
    for(short x=0;x<32;x++) {
        returnVal.append(1,alphas[rand()%(alphas.length()-1)]);
    }
    return returnVal;
}

CURLcode getTrendTweet(std::string trend, std::string* tweetvar) {
    CURL* active=curl_easy_init();
    authencation authStruct;
    struct curl_slist *tokenheader=NULL;
    std::string requestTime=std::to_string(static_cast<long>(std::time(NULL)));
    std::string randomString=getRandomString();
    tokenheader=curl_slist_append(tokenheader,"Content-Type: application/x-www-form-urlencoded");
    std::string host_baseURL="https://api.twitter.com/1.1/users/search.json";
    std::string host=host_baseURL+"?q="+curl_easy_escape(active,trend.c_str(),trend.length());
    std::string host_argument="oauth_consumer_key="+authStruct.consumerKey+"&oauth_nonce="+randomString+"&oauth_signature_method=HMAC-SHA1&oauth_timestamp="+requestTime+"&oauth_token="+authStruct.accessToken+"&oauth_version=1.0&q="+std::string(curl_easy_escape(active,trend.c_str(),trend.length())); //why not just put in parameter?
    std::cout<<host_argument<<"<-host_arg\n";
    std::string signingKey=std::string(curl_easy_escape(active,authStruct.consumerSecret.c_str(),authStruct.consumerSecret.length()))+"&"+std::string(curl_easy_escape(active,authStruct.accessSecret.c_str(),authStruct.accessSecret.length()));
    std::cout<<signingKey<<"<-SigningKey\n";
    std::string signatureBase=std::string("GET&")+curl_easy_escape(active,host_baseURL.c_str(),host_baseURL.length())+std::string("&")+curl_easy_escape(active,host_argument.c_str(),host_argument.length());
    std::cout<<signatureBase<<"<-SignatureBase\n";
    std::cout<<base64::encode(hexStringtoASCII(hmac<SHA1>(signatureBase,signingKey)))<<"<-HEXASII\n";
    std::string signature_=curl_easy_escape(active,base64::encode(hexStringtoASCII(hmac<SHA1>(signatureBase,signingKey).c_str())).c_str(),base64::encode(hexStringtoASCII(hmac<SHA1>(signatureBase,signingKey).c_str())).length());
    std::cout<<signature_<<"<-sig\n";
    std::string oAuthHeader=std::string("Authorization: OAuth oauth_consumer_key=\"")+std::string(curl_easy_escape(active,authStruct.consumerKey.c_str(),authStruct.consumerKey.length()))+std::string("\", oauth_nonce=\""+std::string(curl_easy_escape(active,randomString.c_str(),randomString.length())))+std::string("\", oauth_signature=\"")+signature_+std::string("\", oauth_signature_method=\"HMAC-SHA1\", oauth_timestamp=\"")+requestTime+std::string("\", oauth_token=\"")+std::string(curl_easy_escape(active,authStruct.accessToken.c_str(),authStruct.accessToken.length()))+std::string("\", oauth_version=\"1.0\"");
    std::cout<<oAuthHeader<<"<-OfficialHeader\n";
    tokenheader=curl_slist_append(tokenheader,oAuthHeader.c_str());
    curl_easy_setopt(active,CURLOPT_HTTPGET,1L);
    curl_easy_setopt(active,CURLOPT_URL,host.c_str());
    curl_easy_setopt(active,CURLOPT_USERAGENT,"PostPirate v1.0");
    curl_easy_setopt(active,CURLOPT_WRITEFUNCTION,write_callback);
    curl_easy_setopt(active,CURLOPT_VERBOSE,1L);
    curl_easy_setopt(active,CURLOPT_HTTPHEADER,tokenheader);
    responseData.clear();
    CURLcode response_code=curl_easy_perform(active);
    if(!responseData.empty()&&response_code==NULL) {
        *tweetvar=nlohmann::json::parse(responseData.c_str())[0]["status"]["text"];
        std::cout<<*tweetvar<<":Text\n";
    }
    curl_easy_cleanup(active);
    return response_code;
}

CURLcode getOAuth(std::string* oTokenVar) {
    CURL* active=curl_easy_init();
    struct curl_slist *tokenheader=NULL;
    authencation authStruct;
    tokenheader=curl_slist_append(tokenheader,"oauth_callback=oob");
    std::string ckHeader="oauth_consumer_key="+authStruct.consumerKey;
    tokenheader=curl_slist_append(tokenheader,ckHeader.c_str());
    tokenheader=curl_slist_append(tokenheader,"Accept: application/json");
    tokenheader=curl_slist_append(tokenheader,"Accept-Charset: utf-8");
    tokenheader=curl_slist_append(tokenheader,"Accept-Encoding: identity");   
    curl_easy_setopt(active,CURLOPT_POSTFIELDS,"");
    curl_easy_setopt(active,CURLOPT_URL,"https://api.twitter.com/oauth/request_token");
    curl_easy_setopt(active,CURLOPT_USERAGENT,"PostPirate v1.0");
    curl_easy_setopt(active,CURLOPT_WRITEFUNCTION,write_callback);
    curl_easy_setopt(active,CURLOPT_VERBOSE,0);
    curl_easy_setopt(active,CURLOPT_HTTPHEADER,tokenheader);
    responseData.clear();
    CURLcode response_code=curl_easy_perform(active);
    if(!responseData.empty()&&response_code==NULL) {
        std::cout<<">>>>>"<<responseData<<"<<<<<\n";
    }
    curl_easy_cleanup(active);
    return response_code;
}

std::string hexStringtoASCII(std::string hexString) {
    std::string ASCII;
    for(int x=0;x<hexString.length();x+=2) {
        char asciiCharacter=(char)(int)strtol(hexString.substr(x,2).c_str(),NULL,16);
        ASCII.push_back(asciiCharacter);
    }
    return ASCII;
}

CURLcode postTrendTweet(std::string trend) {
    
}