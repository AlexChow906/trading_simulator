#include <thread>
#include <chrono>

#include "gateway/gateway.h"
#include "strategy/market_maker.h"
#include "strategy/execution_agent.h"
#include "strategy/momentum_agent.h"
#include "strategy/mean_reversion_agent.h"

int main() {
  setbuf(stdout, NULL);
  printf("[sim] starting gateway...\n");
  std::thread server([]{ Gateway gw(8888); gw.run(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  server.detach();

  printf("[sim] connecting market maker...\n");
  MarketMaker mm{"localhost", 8888, "MM", 100.0, 1.0, 1000000};
  printf("[sim] connecting execution agent 1...\n");
  ExecutionAgent ea1{"localhost", 8888, "EA1", 100.50, 5, true, 500, 1};
  printf("[sim] connecting execution agent 2...\n");
  ExecutionAgent ea2{"localhost", 8888, "EA2", 99.50, 3, false, 700, 2000000};
  printf("[sim] connecting momentum agent...\n");
  MomentumAgent ma{"localhost", 8888, "MA", 3, 5, 4000000};
  printf("[sim] connecting mean reversion agent...\n");
  MeanReversionAgent mra{"localhost", 8888, "MRA", 3, 5, 0.25, 6000000};

  printf("[sim] starting agents...\n");
  std::thread mm_thread(&MarketMaker::run, &mm);
  std::thread ea1_thread(&ExecutionAgent::run, &ea1);
  std::thread ea2_thread(&ExecutionAgent::run, &ea2);
  std::thread ma_thread(&MomentumAgent::run, &ma);
  std::thread mra_thread(&MeanReversionAgent::run, &mra);

  sleep(10);

  mm.stop();
  ea1.stop();
  ea2.stop();
  ma.stop();
  mra.stop();

  mm_thread.join();
  ea1_thread.join();
  ea2_thread.join();
  ma_thread.join();
  mra_thread.join();

  mm.print_summary();
  ea1.print_summary();
  ea2.print_summary();
  ma.print_summary();
  mra.print_summary();
}
