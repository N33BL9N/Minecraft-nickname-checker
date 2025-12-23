#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <curl/curl.h>
#include <Windows.h>
#include <cstdlib>
#include <sstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class HttpClient {
private:
    static size_t WriteCallBack(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t sizeall = size * nmemb;
        output->append((char*)contents, sizeall);
        return sizeall;
    }
public:
    HttpClient() { curl_global_init(CURL_GLOBAL_ALL); };
    ~HttpClient() { curl_global_cleanup(); }
    bool Get(std::string& url, std::string& response,long& httpcode) {
        CURL* curl = curl_easy_init();
        if (!curl) { 
        httpcode = 0;
        return false; 
         }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        CURLcode sending = curl_easy_perform(curl);
        if (sending == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
        }
        curl_easy_cleanup(curl);
        return (sending == CURLE_OK);
    }
};
void generatenickname(std::string& isxodnik, int index, std::string process, std::vector<std::string>& results) {
    if (index == isxodnik.length()) {
         results.push_back(process);
         return;
    }
    char c = isxodnik[index];
    /*
    char b;
    if (index + 1 < isxodnik.length()) {
        b = isxodnik[index + 1];
    }
    else b = c;
    */
    if (c == 'e') {
        generatenickname(isxodnik, index + 1, process + '3', results);
        generatenickname(isxodnik, index + 1, process + 'e', results);
    }
    else if (c == 'a') {
        generatenickname(isxodnik, index + 1, process + 'a', results);
        generatenickname(isxodnik, index + 1, process + '4', results);
    }
    else if (c == 'i') {
        generatenickname(isxodnik, index + 1, process + 'i', results);
        generatenickname(isxodnik, index + 1, process + '1', results);
    }
    else if (c == '_' || c == ' ') {
        generatenickname(isxodnik, index + 1, process + '_', results);
        generatenickname(isxodnik, index + 1, process, results);
       // generatenickname(isxodnik, index + 2, process + (char)std::toupper(b), results);
    }
    else if (c == 'l') {
        generatenickname(isxodnik, index + 1, process + 'l', results);
        generatenickname(isxodnik, index + 1, process + '1', results);
        generatenickname(isxodnik, index + 1, process + '!', results);
    }
    else if (c == 'o') {
        generatenickname(isxodnik, index + 1, process + '0', results);
        generatenickname(isxodnik, index + 1, process + 'o', results);
    }
    else if (c == 'k' || c == 'c') {
        generatenickname(isxodnik, index + 1, process + 'k', results);
        generatenickname(isxodnik, index + 1, process + 'c', results);
    }
    else if (c == 's') {
        generatenickname(isxodnik, index + 1, process + 's', results);
        generatenickname(isxodnik, index + 1, process + '5', results);
    }
    else if (c == 'b') {
        generatenickname(isxodnik, index + 1, process + 'b', results);
        generatenickname(isxodnik, index + 1, process + '6', results);
    }
    else if (c == 'z') {
        generatenickname(isxodnik, index + 1, process + 'z', results);
        generatenickname(isxodnik, index + 1, process + '2', results);
    }
    else if (c == 'w') {
        generatenickname(isxodnik, index + 1, process + 'w', results);
        generatenickname(isxodnik, index + 1, process + "vv", results);
    }
    else { generatenickname(isxodnik, index + 1, process + c, results); }
}
void SaveToFile(std::vector<std::string>& resultnicks) {
    std::ofstream b;
    b.open("result.txt");
    if (b.is_open()) {
        for (int i = 0; i < resultnicks.size(); i++) b << resultnicks[i] << '\n';
    }
    b.close();
}
void LogicProgram() {
    HttpClient hc;
    std::string nick;
    std::vector<std::string> result;
    std::vector<std::string> resultnicks;
    std::string process;
    std::string url = "https://api.mojang.com/users/profiles/minecraft/";
    unsigned const int WaitingForRequest = 1200;
    unsigned const int MinSymbols = 3;
    unsigned const int MaxSymbols = 16;
    int count = 1;
    std::cout << "Введите исходник: ";
    std::getline(std::cin, nick);
    if (nick.length() < MinSymbols || nick.length() > MaxSymbols)
    {
        std::cout << "длина ника должна быть не меньше 3 и не больше 16!" << '\n';
        std::getline(std::cin, nick);
    }
    std::transform(nick.begin(), nick.end(), nick.begin(), ::tolower);
    generatenickname(nick, 0, process, result);
    for (std::string& c : result) {

        std::string fullurl = url + c;
        std::string resp;
        long httpcode;

        hc.Get(fullurl, resp, httpcode);
        json data = json::parse(resp);
        std::string id;

        try {
            if (data["id"].empty() && !data["id"].is_string()) {

                throw std::string{ "Произошла ошибка при получении id!" };
            }
            id = data["id"];
        }
        catch (std::string errortext) {
            if (httpcode != 204 && httpcode != 404) {
                std::cerr << errortext;
            }
        }

        if (httpcode == 204 || httpcode == 404) {
            resultnicks.push_back(c);
            std::cout << '[' << count << '|' << result.size() << ']' << " + свободный ник: " << c << '\n' << '\n';
            count++;
        }
        else { std::cout << '[' << count << '|' << result.size() << ']' << " - ник занят: " << c << ' ' << "\n" << "айди профиля: " << id << '\n' << '\n'; count++; }
        Sleep(WaitingForRequest);
    }
    SaveToFile(resultnicks);
}
int main()
{
    system("color 02");
    setlocale(LC_ALL, "ru");
    char ExitFromProgram;
    LogicProgram();
    std::cout << '\n' << "Все свободные ники сохранены в result.txt!";
    std::cin >> ExitFromProgram;
    return 0;
}
