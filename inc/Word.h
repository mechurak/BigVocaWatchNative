#ifndef WORD_H_
#define WORD_H_

#include <string>

struct Word {
	int id;
	int day;
	std::string spelling;
	std::string phonetic;
	std::string meaning;
};

#endif /* WORD_H_ */
