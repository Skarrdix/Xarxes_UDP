#include "PacketLoss.h"

int PacketLoss::generate_prob()
{
	return (rand() % 1000) + 1; // Returns a random int value between 1 and 1000
}
