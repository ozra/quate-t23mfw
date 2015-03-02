#include "RequestUrl.hh"
#ifdef INTERFACE

    /**
    * Created:  2014-12-28
    * Author:   Oscar Campbell
    * Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
    **/

    #include <curl/curl.h>
    #include "MutatingBuffer.hh"


#endif

#define local static

local size_t
on_http_data(void * contents, size_t size, size_t nmemb, void * user_data)
{
    size_t real_size = size * nmemb;
    MutatingBuffer<char> * buf = static_cast<MutatingBuffer<char>*>(user_data);
    buf->append(contents, real_size);
    return real_size;
}

int http_request(char const * uri, MutatingBuffer<char> * io_result,
                 int retries = 10)
{
    CURL * curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "Mozilla/5.0 (Linux; T23M) T23MFW/0.47 QBAPI/0.47");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_http_data);
    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    io_result->append('\0');
    /* Check for errors */
    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
