#include "mongoose.h"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

static const char *json = "{\"message\":\"hey\"}";
static const char *defaultPort = "8080";

static void ev_handler(struct mg_connection *conn, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_REQUEST) {
    mg_send_head(conn, 200, strlen(json), "Content-Type: application/json");
    mg_printf(conn, "%s", json);
    conn->flags |= MG_F_SEND_AND_CLOSE;
  }
}

static const char* getPort() {
  char *portFromEnv = std::getenv("PORT");
  if (portFromEnv == nullptr) {
    return std::string("8080").c_str();
  }
  return portFromEnv;
}

int startServer() {
  struct mg_mgr mgr;
  struct mg_connection *conn;
  mg_mgr_init(&mgr, nullptr);

  const char *port = getPort();
  conn = mg_bind(&mgr, port, ev_handler);

  if (conn == nullptr) {
    std::cout << "failed to start server" << std::endl;
    return 1;
  }

  std::cout << "started server on port " << port << std::endl;
  mg_set_protocol_http_websocket(conn);

  while (1) {
    mg_mgr_poll(&mgr, 50);
  }

  mg_mgr_free(&mgr);
  return 0;
}

int main() {
  startServer();
  return 0;
}
