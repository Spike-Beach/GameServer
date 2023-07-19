#include <iostream>
#include <winsock2.h>
#include <hiredis.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

int main()
{
	std::cout << "Hello world" << std::endl;
    redisContext* c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            // handle error
        }
        else {
            printf("Can't allocate redis context\n");
        }
    }
	return 0;
}