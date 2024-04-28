#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Auth Func (Return : 1(Clear), -1(Fail))
int auth(Login USER) {
    
    MYSQL *connection = NULL, conn;
    MYSQL_RES *sql_result;
    MYSQL_ROW sql_row;

    int query_stat;                 // Store query result status
    char db_name[8] = "ACCOUNT";    // DB name
    char table_name[5] = "USER";    // DB table name
    
    char ID[10];
    char PASSWD[16];
    char QUERY[100];
    int result;
    
    mysql_init(&conn);  // MySQL Init
    
    connection = mysql_real_connect(&conn, cfg.HOST, cfg.USER, cfg.PASS, db_name, cfg.PORT, (char *)NULL, 0);  // Connect to Database
    
    // If fail to connection, output error message
    if (connection == NULL) {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }
    
    // Make Query 
    sprintf(QUERY, "select PASSWD from %s where ID=\"%s\"", table_name, USER.ID);
    // Query to database
    query_stat = mysql_query(connection, QUERY);
    
    #ifdef __DEBUG_
    printf("%s\n",QUERY);
    #endif
    
    // If fail to query, output error message
    if (query_stat != 0) {
        fprintf(stderr, "Mysql query error : %s\n", mysql_error(&conn));
        
        mysql_close(connection);
        return -1;
    }
    
    // Store result
    sql_result = mysql_store_result(connection);
    // Fetch rows
    sql_row = mysql_fetch_row(sql_result);

    // If ID not exist on database, return -1(Auth Fail)
    if(sql_row == NULL) {
        #ifdef __DEBUG_
        printf("No User Find.\n");
        #endif
        
        return -1;
    }
    
    #ifdef __DEBUG_
    printf("Auth : %s %s Check\n", sql_row[0], USER.PW);
    #endif
    
    // Auth process start
    // If PASSWD match, return 1(Auth Clear)
    if(!strcmp(sql_row[0], USER.PW)) {
        #ifdef __DEBUG_
        printf("Auth Clear.\n");
        #endif
        
        result = 1;
    }
    // If PASSWD not match, return -1(Auth Fail)
    else {
        #ifdef __DEBUG_
        printf("Auth Fail.\n");
        #endif
        
        result = -1;
    }
    //Auth process End
    
    mysql_free_result(sql_result);
    mysql_close(connection);
    return result;
}