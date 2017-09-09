//
// Created by symbx on 09.09.17.
//

#include <bits/move.h>
#include <core/Server.h>
#include "core/Query.h"

namespace uGame {
    Query::Query(MYSQL_RES *res) {
        _res = res;
        _rows = static_cast<long>(mysql_num_rows(res));
    }

    Query::~Query() {
        mysql_free_result(_res);
    }

    long Query::getRowsCount() {
        return _rows;
    }

    MYSQL_ROW Query::getRow() {
        MYSQL_ROW row = mysql_fetch_row(_res);
        return std::move(row);
    }
}