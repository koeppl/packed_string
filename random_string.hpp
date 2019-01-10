#pragma once

#include <cstring>
#include <algorithm>

size_t random_size(const size_t& maxvalue) {
   return static_cast<std::size_t>(std::rand() * (1.0 / (RAND_MAX + 1.0 )) * maxvalue);
}

struct random_char{
    random_char(char const* range = "abcdefghijklmnopqrstuvwxyz0123456789")
        : m_range(range), m_length(std::strlen(m_range)) { }

    char operator ()() const {
        return m_range[random_size(m_length)];
    }

private:
    char const*const m_range;
    const std::size_t m_length;
};

std::string random_string(const random_char& rnd_gen, size_t length) {
	std::string s(length, '\0');
	std::generate_n(s.begin(), length, rnd_gen);
	// s[length] = '\0';
	return s;
}

