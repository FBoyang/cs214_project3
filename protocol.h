#ifndef _PROTOCOL_H
#define _PROTOCOL_H

const char *const DUMP_PREFIX = "QUIT_SERVER";
const char *const DUMP_FMT = "QUIT_SERVER-_-%d";
const int DUMP_PREFIX_LEN = 11;
const char *const LENGTH_FMT = "length %ld";
const char *const NEW_SESSION_PREFIX = "Get_Id";
const char *const NEW_SESSION_FMT = "Get_Id-_-%s";
const int NEW_SESSION_PREFIX_LEN = 6;
const char *const SESSION_ID_FMT = "%ld";
const char *const SORT_FMT = "%ld-_-%d";
const char *const FILE_DONE_FMT = "done";


#endif
