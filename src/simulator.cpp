#include <thread>
#include <chrono>

#include "gateway/gateway.h"
#include "strategy/market_maker.h"
#include "strategy/execution_agent.h"

int main() {
  setbuf(stdout, NULL);
  printf("[sim] starting gateway...\n");
  std::thread server([]{ Gateway gw(8888); gw.run(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  server.detach();

  printf("[sim] connecting market maker...\n");
  MarketMaker mm{"localhost", 8888, 100.0, 1.0, 1000000};
  printf("[sim] connecting execution agent 1...\n");
  ExecutionAgent ea1{"localhost", 8888, 100.50, 5, true, 500, 1, "EA1"};
  printf("[sim] connecting execution agent 2...\n");
  ExecutionAgent ea2{"localhost", 8888, 99.50, 3, false, 700, 2000000, "EA2"};

  printf("[sim] starting agents...\n");
  std::thread mm_thread(&MarketMaker::run, &mm);
  std::thread ea1_thread(&ExecutionAgent::run, &ea1);
  std::thread ea2_thread(&ExecutionAgent::run, &ea2);

  mm_thread.join();
  ea1_thread.join();
  ea2_thread.join();
}
