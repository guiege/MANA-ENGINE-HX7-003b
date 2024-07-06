#pragma once

#include <iostream>
#include <vector>
#include <bitset>

class CommandSequence{
public:

	int commandTimer = 0;

	int commandNumber = 0;

	int currentTick = 0;

	int lastInputTick = 0;

	int tE;

	std::vector<std::bitset<4>> commandList;

	std::vector<int> holdList;

	CommandSequence(const std::vector<std::bitset<4>> &commandList, const std::vector<int> &holdList, int tE)
	:commandList(commandList), holdList(holdList), tE(tE)
	{}

};