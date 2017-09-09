//
// Created by symbx on 09.09.17.
//

#ifndef UGAMEMASTERSERVER_QUERY_H
#define UGAMEMASTERSERVER_QUERY_H

#include <mysql.h>
#include <string>

namespace uGame {
    class Query {
    public:
        Query(MYSQL_RES* res);
        ~Query();
        long getRowsCount();
        MYSQL_ROW getRow();

    protected:
        MYSQL_RES* _res;
        long _rows;

    };
}

#endif //UGAMEMASTERSERVER_QUERY_H
