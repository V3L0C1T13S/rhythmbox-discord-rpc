#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <iomanip>
#include <unordered_map>
#include <openssl/sha.h>
#include <chrono>

struct CachedFile {
    std::string url;
    std::chrono::time_point<std::chrono::system_clock> expires_at;
};

std::unordered_map<std::string, CachedFile> upload_cache;

std::string calculateFileHash(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    while (file.read(buffer, bufferSize)) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    SHA256_Update(&sha256, buffer, file.gcount());

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::ostringstream oss;
    for (unsigned char byte : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }

    return oss.str();
}

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

    auto hash = calculateFileHash(filename);
    auto existing_file_it = upload_cache.find(hash);
    if (existing_file_it != upload_cache.end()) {
        auto existing_file = existing_file_it->second;
        if (existing_file.expires_at > std::chrono::system_clock::now()) {
            return existing_file.url;
        }
    }

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
        auto fileContents = oss.str();

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

        using namespace std::chrono;
        upload_cache.insert({ hash, CachedFile({
            .url = response,
            .expires_at = system_clock::now() + hours(1),
        }) });
    }
    else
    {
        throw std::runtime_error("Unable to initialize CURL.");
    }

    return response;
}
