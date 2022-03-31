#include <iostream>

#include "../cpp-httplib/httplib.h"


int main(void)
{
    httplib::Client cli("localhost", 8080);

    if (auto res = cli.Get("/hi")) {
        if (res->status == 200) {
            std::cout << res->body << std::endl;
        }
    }
    else 
    {
        auto err = res.error();
    }
}