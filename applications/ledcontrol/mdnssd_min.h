#ifndef __MDNSSD_MIN_H__
#define __MDNSSD_MIN_H__

#define DNS_HEADER_SIZE (12)
#define DNS_MAX_HOSTNAME_LENGTH (253)
#define DNS_MAX_LABEL_LENGTH (63)
#define MDNS_MULTICAST_ADDRESS "224.0.0.251"
#define MDNS_PORT (5353)
// #define DNS_BUFFER_SIZE (32768)
#define DNS_BUFFER_SIZE (2048)

// TODO find the right number for this
//#define DNS_MESSAGE_MAX_SIZE (4096)
#define DNS_MESSAGE_MAX_SIZE (2048)

// DNS Resource Record types
// (RFC 1035 section 3.2.2)
#define DNS_RR_TYPE_A (1)
#define DNS_RR_TYPE_CNAME (5)
#define DNS_RR_TYPE_PTR (12)
#define DNS_RR_TYPE_TXT (16)
#define DNS_RR_TYPE_SRV (33)

// TODO not sure about this
// #define MAX_RR_NAME_SIZE (256)
#define MAX_RR_NAME_SIZE (128)

#define MAX_DEREFERENCE_COUNT (40)

// The maximum number of answers allowed
// #define MAX_ANSWERS (200)
#define MAX_ANSWERS (20)

#define MAX_RUNTIME (86400)

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


struct mDNSMessageStruct{
  uint16_t id;
  uint16_t flags;
  uint16_t qd_count;
  uint16_t an_count;
  uint16_t ns_count;
  uint16_t ar_count;
  char* data;
  size_t data_size;
} __attribute__((__packed__)); // ensure that struct is packed
typedef struct mDNSMessageStruct mDNSMessage;

typedef struct {
  int qr;
  int opcode;
  int aa;
  int tc;
  int rd;
  int ra;
  int zero;
  int ad;
  int cd;
  int rcode;
} mDNSFlags;

typedef struct {
  char* qname;
  uint16_t qtype;
  uint16_t qclass;
  int prefer_unicast_response;
} mDNSQuestion;

typedef struct {
  char* name;
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  uint16_t rdata_length;
  void* rdata;
} mDNSResourceRecord;

typedef struct {
  mDNSResourceRecord* rr; // the parent RR
  char* name; // name from PTR
  char* hostname; // from SRV
  struct in_addr addr; // from A
  unsigned short port; // from SRV
  int srv_query_sent; // has a srv query already been sent
  int a_query_sent; // has an a query already been sent
} FoundAnswer;

typedef struct {
  // TODO should use linked list?
  FoundAnswer* answers[MAX_ANSWERS]; 
  size_t length;
  size_t completed_length; // number of complete answers (answers that have both an IP and a port number)
} FoundAnswerList;

// int debug(const char* format, ...);
void usage(char* argv0);
void fail(char* msg);
void init_answer_list(FoundAnswerList* alist);
FoundAnswer* add_new_answer(FoundAnswerList* alist);
FoundAnswer* add_answer(FoundAnswerList* alist, FoundAnswer* a);
void clear_answer_list(FoundAnswerList* alist);
char* prepare_query_string(char* name);
char* parse_hostname(char* hostname);
mDNSMessage* mdns_make_message();
mDNSFlags* mdns_parse_header_flags(uint16_t data);
uint16_t mdns_pack_header_flags(mDNSFlags flags);
char* mdns_pack_question(mDNSQuestion* q, size_t* size);
void mdns_message_print(mDNSMessage* msg);
int mdns_parse_question(char* message, char* data, int size);
int mdns_parse_rr_a(char* data, FoundAnswer* a);
int mdns_parse_rr_ptr(char* message, char* data, FoundAnswer* a);
int mdns_parse_rr_srv(char* message, char* data, FoundAnswer* a);
uint16_t get_offset(char* data);
char* parse_rr_name(char* message, char* name, int* parsed);
void mdns_parse_rdata_type(char* message, mDNSResourceRecord* rr, FoundAnswer* answer);
void free_resource_record(mDNSResourceRecord* rr);
int mdns_parse_rr(char* message, char* rrdata, int size, FoundAnswerList* alist, int is_answer);
int mdns_parse_message_net(char* data, int size, mDNSMessage* msg, FoundAnswerList* alist);
mDNSMessage* mdns_build_query_message(char* query_str, uint16_t query_type);
char* mdns_pack_message(mDNSMessage* msg, size_t* pack_length);
int parse_received(char* data, int size, FoundAnswerList* alist);
int get_arg(int argc, char* argv[], char* key, char** val);
int out_of_time(struct timeval* t);
// void print_answers(FoundAnswerList* alist);
int print_answers(FoundAnswerList* alist, char *ip, int *port);
int send_query(int sock, char* query_arg, uint16_t query_type);
int init_socket();
int is_answer_complete(FoundAnswer* a);
void complete_answer(int sock, FoundAnswerList* alist, FoundAnswer* a);
void complete_answers(int sock, char* query_arg, FoundAnswerList* alist);
void main_loop(int sock, char* query_arg, int min_answers, FoundAnswerList* alist, struct timeval* runtime);
// int main(int argc, char* argv[]);

int getService(char * srv_name, char *ip, int *port);


#endif