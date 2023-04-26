#include <CLI/CLI.hpp>
#include <aglio/format.hpp>
#include <fmt/format.h>
#include <map>
#include <slook/boost_asio.hpp>
#include <slook/slook.hpp>

struct ServiceDiscovery {
    using Clock = std::chrono::steady_clock;

    boost::asio::io_context&                 ioc;
    boost::asio::basic_waitable_timer<Clock> timer;
    slook::AsioServer                        slookServer;
    std::string                              searchPattern;

    static constexpr auto SerachInterval  = std::chrono::seconds{1};
    static constexpr auto TimeoutInterval = std::chrono::seconds{5};
    static constexpr auto StartInterval   = std::chrono::milliseconds{100};

    std::map<slook::AsioServer::Lookup_t::Service, Clock::time_point> services;

    ServiceDiscovery(boost::asio::io_context& ioc_, std::string const& searchPattern_)
      : ioc{ioc_}
      , timer{ioc}
      , slookServer{ioc, 7331, "239.255.13.37"}
      , searchPattern{searchPattern_} {
        start(StartInterval);
    }

    void start(auto const& interval) {
        timer.expires_at(Clock::now() + interval);
        timer.async_wait([this](auto ec) { timeout(ec); });
    }

    void timeout(boost::system::error_code ec) {
        if(!ec) {
            std::erase_if(services, [](auto const& entry) {
                auto const& [service, time] = entry;
                if(Clock::now() > time + TimeoutInterval) {
                    fmt::print("lost: {}\n", service);
                    return true;
                }
                return false;
            });

            slookServer.getLookup().findServices(searchPattern, [&](auto const& foundService) {
                auto const it = services.find(foundService);
                if(it != services.end()) {
                    it->second = Clock::now();
                } else {
                    fmt::print("found: {}\n", foundService);
                    services[foundService] = Clock::now();
                }
            });

            start(SerachInterval);
        }
    }
};

int main(int argc, char** argv) {
    CLI::App app{"slook_test"};

    std::string pattern{};
    app.add_option("--pattern", pattern, "the service serach pattern");

    try {
        app.parse(argc, argv);
    } catch(CLI::ParseError const& e) {
        return app.exit(e);
    }

    boost::asio::io_context ioc;

    ServiceDiscovery serviceDiscovery(ioc, pattern);

    ioc.run();
}
