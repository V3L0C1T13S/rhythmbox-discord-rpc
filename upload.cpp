#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string uploadFile(const std::string &filename)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::ostringstream oss;
        oss << file.rdbuf();
        std::string fileContents = oss.str();

        curl_mime *mime;
        curl_mimepart *part;

        mime = curl_mime_init(curl);

        part = curl_mime_addpart(mime);
        curl_mime_name(part, "reqtype");
        curl_mime_data(part, "fileupload", CURL_ZERO_TERMINATED);

        part = curl_mime_addpart(mime);
        curl_mime_name(part, "time");
        curl_mime_data(part, "1h", CURL_ZERO_TERMINATED);

        part = curl_mime_addpart(mime);
        curl_mime_name(part, "fileToUpload");
        curl_mime_filename(part, filename.c_str());
        curl_mime_data(part, fileContents.c_str(), fileContents.size());

        curl_easy_setopt(curl, CURLOPT_URL, "https://litterbox.catbox.moe/resources/internals/api.php");
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            curl_mime_free(mime);
            curl_easy_cleanup(curl);
            throw std::runtime_error("Curl error: " + std::string(curl_easy_strerror(res)));
        }

        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }
    else
    {
        throw std::runtime_error("Unable to initialize CURL.");
    }

    return response;
}
