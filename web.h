#ifndef FUNCTIONALBADGES_WEB_H
#define FUNCTIONALBADGES_WEB_H

#include <string>
#include <atomic>

#include <boost/uuid/uuid.hpp>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <boost/uuid/uuid_io.hpp>


inline void sendData(std::string first, std::string last, std::string id, uint8_t grade, boost::uuids::uuid uuid) {

    std::string body = first + "," + last + "," + id + "," + std::to_string(grade) + "," + boost::uuids::to_string(uuid);

    Pistache::Http::Client client;

    auto opts = Pistache::Http::Client::options()
                                       .threads(1)
                                       .maxConnectionsPerHost(1);
    client.init(opts);

    client.put("http://localhost:8080")
          .body(body)
          .send()
          .then([&client](Pistache::Http::Response response) {
              switch (response.code()) {
                  case Pistache::Http::Code::Ok:
                      std::cout << "Server accepted entry (" << response.body() << ")" << std::endl;
                      break;
                  case Pistache::Http::Code::Forbidden:
                      std::cout << "Server rejected entry (" << response.body() << ")" << std::endl;
                      break;
                  default:
                      std::cout << "Server responded with unexpected " << response.code() << std::endl;
              }
              client.shutdown();
          }, [&client](std::exception_ptr x) {
              std::cerr << "Connection exception: " << x.__cxa_exception_type() << std::endl;
             client.shutdown();
          });
}

#endif //FUNCTIONALBADGES_WEB_H
