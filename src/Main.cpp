#include "mongoose.h"
#include <cstdlib>
#include <string>
#include <iostream>

static const char *json = "{\"message\":\"hey\"}";
static const char *defaultPort = "8080";
static int s_sig_num = 0;

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}

static void ev_handler(struct mg_connection *conn, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_REQUEST) {
    mg_send_head(conn, 200, strlen(json), "Content-Type: application/json");
    mg_printf(conn, "%s", json);
    conn->flags |= MG_F_SEND_AND_CLOSE;
  }
}

static const char * getPort() {
  char * portFromEnv = std::getenv("PORT");

  if (portFromEnv == NULL) {
    return std::string("8080").c_str();
  } else {
    return portFromEnv;
  }
}

int startServer() {
  struct mg_mgr mgr;
  struct mg_connection *conn;
  mg_mgr_init(&mgr, NULL);

  const char *port = getPort();
  conn = mg_bind(&mgr, port, ev_handler);

  if (conn == NULL) {
    std::cout << "failed to start server" << std::endl;
    return 1;
  }

  mg_set_protocol_http_websocket(conn);
  std::cout << "started server on port " << port << std::endl;

  while (s_sig_num == 0) {
    mg_mgr_poll(&mgr, 50);
  }

  mg_mgr_free(&mgr);
  return 0;
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  startServer();
  return 0;
}
