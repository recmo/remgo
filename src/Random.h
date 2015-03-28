#pragma once
#include "Utilities.h"
#include <random>

float randomReal();

uint32 entropy(uint upperBound);

// true = white
bool blackOrWhite(uint numBlack, uint numWhite);
