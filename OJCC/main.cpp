#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "EquityCalculator.h"

using namespace omp;
using namespace std;

#define POT_ODDS 0.46

bool is_profitable(const CardRange& villain_range, const CardRange& heros_hand, double po) {
	EquityCalculator eq;
	eq.start({ villain_range, heros_hand });
	eq.wait();

	EquityCalculator::Results results = eq.getResults();

	return results.equity[1] > po;
}

void eval_jam_calling() {
	string villain_jamming_range = "A3s, J3s, 33, A4s, 44, 44, 66, 77, 87s, 87s, 87o, Q8s, K8s, 99, K9s, TT, TT, JTs, ATs, JJ, JJ, QJs, KJs, QQ, A3o, A6o, A8o, ATo, AJo, AJo, AQo, AQo, AQo, AQo, AKo, AKo, AKo, AA, AKs, KK, KQo, KJo, KJo, KTo, K9o";

	EquityCalculator eq;
	CardRange villain(villain_jamming_range);

	int ranks = 13;
	char letters[] = "23456789TJQKA";

	for (int x = 0; x < ranks; x++) {
		for (int y = x; y < ranks; y++) {
			std::vector<std::string> suffixes = (x == y ? std::vector<std::string>{""} : std::vector<std::string>{ "s", "o" });

			for (std::string s : suffixes) {
				std::string hand = std::string(1, letters[x]) + std::string(1, letters[y]) + s;
				CardRange hero (hand);
				if (is_profitable(villain, hero, POT_ODDS)) {
					std::cout << hand << ", ";
				}
			}
		}
	}
	std::cout << std::endl;
	
}

std::string idx_to_name(uint8_t idx) {
	char letters[] = "23456789TJQKA";
	char suits[] = "shcd";
	return std::string(1, letters[idx >> 2]) + std::string(1, suits[idx & 3]);
}

void equity_trainer() {
	srand(time(0));

#define UTG_OPEN "55,66,77,88,99,TT,JJ,QQ,KK,AA,A3s,A4s,A5s,A6s,A7s,A8s,A9s,ATs,AJs,AQs,AKs,K5s,K6s,K7s,K8s,K9s,KTs,KJs,KQs,Q9s,QTs,QJs,J9s,JTs,T9s,ATo,AJo,AQo,AKo,KJo,KQo"
#define BTN_CALL "JJ,TT,99,88,77,66,55,44,AJs,ATs,A9s,A8s,A5s,A4s,A3s,A2s,KTs,KJs,KQs,QTs,QJs,JTs,T9s,AQo"
#define BB_CALL "JJ,TT,99,88,77,66,55,44,33,22,AQs,AJs,ATs,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,K2s,K3s,K4s,K5s,K6s,K7s,K8s,K9s,KTs,KJs,KQs,Q2s,Q3s,Q4s,Q5s,Q6s,Q7s,Q8s,Q9s,QTs,QJs,J4s,J5s,J6s,J7s,J8s,J9s,JTs,T6s,T7s,T8s,T9s,95s,96s,97s,98s,85s,86s,87s,74s,75s,76s,63s,64s,65s,52s,53s,54s,42s,43s,32s,AQo,AJo,ATo,A9o,A8o,A5o,KTo,KJo,KQo,QTo,QJo,JTo,T9o"
	
	CardRange utg(UTG_OPEN);
	CardRange other(BTN_CALL);

	while (1) {
		std::vector<uint8_t> cards;
		for (int x = 0; x < 52; x++) {
			cards.push_back(x);
		}

		int index1 = rand() % 52;
		uint8_t card1 = cards[index1];
		cards.erase(cards.begin() + index1);
		int index2 = rand() % 51;
		uint8_t card2 = cards[index2];
		cards.erase(cards.begin() + index2);
		int index3 = rand() % 50;
		uint8_t card3 = cards[index3];
		cards.erase(cards.begin() + index3);

		std::cout << "Board: " << idx_to_name(card1) << " " << idx_to_name(card2) << " " << idx_to_name(card3) << std::endl;

		uint64_t board = (1ULL << card1) | (1ULL << card2) | (1ULL << card3);

		EquityCalculator eq;
		eq.start({utg, other }, board);
		eq.wait();
		EquityCalculator::Results results = eq.getResults();

		double guessed_eq = 0;
		std::cin >> guessed_eq;

		double error = abs(guessed_eq - results.equity[0]);

		if (error < 0.015) {
			std::cout << "Excellent! ";
		} else if (error < 0.03) {
			std::cout << "Good! ";
		}  else {
			std::cout << "Try again! ";
		}
		std::cout << results.equity[0] << std::endl;
	}

}

int main() {


	equity_trainer();

	int x;
	cin >> x;

	return 0;
}