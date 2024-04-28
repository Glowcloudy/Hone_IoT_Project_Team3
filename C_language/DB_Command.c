#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

void db_Command(const DB_Command com) {
    MYSQL       *connection=NULL, conn;
    MYSQL_RES   *sql_result;
    MYSQL_ROW   sql_row;

    int query_stat;
    int result;
    char QUERY[100];

    // MySQL init
    mysql_init(&conn);
    
    // Connect to MySQL Server
    connection = mysql_real_connect(&conn, cfg.HOST,
                                    cfg.USER, cfg.PASS,
                                    com.db_name, cfg.PORT,
                                    (char *)NULL, 0);
    
    // if fail to connect, output error message and exit function
    if (connection == NULL) {
        printf("Mysql connection error : %s", mysql_error(&conn));
        return;
    }

    // Command Check(LIST)
    // SELECT operation(USER views)
    if(!strcmp(com.command, "SELECT")) {

        // Query Set
        sprintf(QUERY, "select ID, PASSWD from %s", com.table_name);
        // Query to MySQL server
        query_stat = mysql_query(connection, QUERY);
        
        #ifdef __DEBUG_
        printf("%s\n", QUERY);
        #endif

        // if fail to Query, output error message and exit function
        if (query_stat != 0) {
            printf("Mysql query error : %s", mysql_error(&conn));
            
            mysql_close(connection);
            return;
        }
        
        // Store query result
        sql_result = mysql_store_result(connection);

        // Print
        while(sql_row = mysql_fetch_row(sql_result)) {
            printf("%s  %s \n", sql_row[0], sql_row[1]);
        }

        // delete result from memory
        mysql_free_result(sql_result);
        // connection close
        mysql_close(connection);
    }
    // DELETE operation(USER delete)
    else if(!strcmp(com.command, "DELETE")) {
        if(strcmp(com.arg_name, "NULL")) {

            // Query Set
            sprintf(QUERY, "delete from %s where ID=\"%s\"", com.table_name, com.arg_name);
            // Query to MySQL server
            query_stat = mysql_query(connection, QUERY);

            // if success to query, output message
            if(query_stat == 0) {
                printf("유저 %s가 데이터베이스에서 삭제되었습니다.\n", com.arg_name);
            }
            // if fail to Query, output error message
            else {
                printf("삭제에 실패하였습니다.\n");
                printf("쿼리 : %s\n", QUERY);
            }

            // connection close
            mysql_close(connection);
        }
    }
    // INSERT operation('USER insert' or 'SENSOR data insert')
    else if(!strcmp(com.command, "INSERT")) {
        // 'USER INSERT' operation
        if(strcmp(com.arg_name, "NULL")) {

            // Query Set
            sprintf(QUERY, "insert into %s (ID, PASSWD) values ('%s', '%s')", com.table_name, com.arg_name, com.arg_pass);
            // Query to MySQL server
            query_stat = mysql_query(connection, QUERY);

            // if success to query, output message
            if(query_stat == 0) {
                printf("유저 %s가 데이터베이스에 추가되었습니다.\n", com.arg_name);
            }
            // if fail to Query, output error message
            else {
                printf("추가에 실패하였습니다.\n");
                printf("쿼리 : %s\n", QUERY);
            }

            // connection close
            mysql_close(connection);
        }
        // 'SENSOR data insert'
        else {
            // If not exist value3
            if(com.value3 == 999) {
                sprintf(QUERY, "insert into %s (time, value) values ('%d', '%d')", com.table_name, com.value1, com.value2);  
            }
            // else
            else {
                sprintf(QUERY, "insert into %s (time, humi, temp) values ('%d', '%d', '%d')", com.table_name, com.value1, com.value2, com.value3);
            }

            // Query to MySQL server
            query_stat = mysql_query(connection, QUERY);

            // if fail to Query, output error message
            if(query_stat != 0) {
                printf("%s에 데이터 추가 실패\n", com.db_name);
            }

            // connection close
            mysql_close(connection);
        }
    }
}