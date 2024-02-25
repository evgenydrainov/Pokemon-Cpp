#pragma once

template <typename T>
static T min(T a, T b) {
	return (a < b) ? a : b;
}

template <typename T>
static T max(T a, T b) {
	return (a > b) ? a : b;
}

template <typename T>
static T clamp(T x, T a, T b) {
	return max(min(x, b), a);
}

template <typename T>
static T wrap(T a, T b) {
	return (a % b + b) % b;
}
