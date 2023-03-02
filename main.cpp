#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <random>
#include <algorithm>
/*
sctruct Symbol is used for task2
uint8_t sym is character with code from 0 to 255
double freq is frequency of meeting character in the text
*/
struct Symbol {
	uint8_t sym;
	double freq;
};
/*
pos1 is byte number
pos2 is byte number for change
*/
struct Positions {
	uint8_t pos1;
	uint8_t pos2;
};
/*
is overload of operator<< for simplicity of printing struct Symbol
*/
std::ostream& operator<<(std::ostream& os, const Symbol & obj);
/*
function for choosing between tasks 1, 2, 3 and handling incorrect input
*/
uint8_t choose_mode();
/*
function for task1
finds length of phrase in .doc file
*/
void task1();
/*
function for task2
finds relative frequncies of meeting bytes in the file
*/
void task2();
/*
function for task3
encrypts and deencrypts file
*/
void task3();
/*
function finds number of bytes in file named std::string filename
*/
uint64_t num_of_bytes(const std::string& filename);
/*
function reads user input and returns std::string filename
*/
std::string input_filename();
/*
generates key.txt file for encrypting and deencrypting
returns true, when file generated
returns false, when not
*/
bool generate_key();
/*
checks if there are already this key in generated vector
returns true if yes
return false if no
*/
bool check_keys(std::vector<int32_t>& vec, const int32_t x);
/*
encrypts file
true if ok
false if error occuried
*/
bool encrypt();
/*
encrypts file
true if ok
false if error occuried
*/
bool decipher();
/*
reads keys.txt
invert true if decipher, need to invert keys
false if encrypting
*/
std::vector<Positions> read_keys(bool invert);
/*
changes content if size of last part < 256 leaves without changes
pos is vector with read keys
filename is name of file
type is true for encoding, false for decipher
true if is ok
false if smth wrong
*/
bool change_content(const std::vector<Positions>& pos, const std::string& filename, const std::string& prefix);

int main() {
	choose_mode();
	return 0;
}

std::ostream& operator<<(std::ostream& os, const Symbol& obj)
{
	os << obj.sym << ' ' << obj.freq << '\n';
	return os;
}

uint8_t choose_mode()
{
	uint32_t mode;
	std::cout << "Please choose mode: \n 1. Task 1\n 2. Task 2\n 3. Task 3\n";
	std::cin >> mode;
	if (std::cin.good()) {
		switch (mode)
		{
		case 1:
			task1();
			break;
		case 2:
			task2();
			break;
		case 3:
			task3();
			break;
		default:
			std::cout << "Incorrect input\n";
			std::cin.clear(); 
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return choose_mode();
			break;
		}
	}
	else {
		std::cout << "Incorrect input\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return choose_mode();
	}
	return mode;
}

void task1()
{
	std::string filename = "lab1a.doc";
	std::cout << "Length in bytes: " << num_of_bytes(filename) << '\n';
}

void task2()
{
	std::fstream input(input_filename(), std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		std::cout << "Cannot open file\n";
		task2();
	}
	else {
		Symbol alf[256];
		char t;
		uint64_t size = 0;
		for (uint16_t i = 0; i != 256; i++) {
			alf[i].sym = (uint8_t) i;
			alf[i].freq = 0.0;
		}
		while (!input.eof()) {
			input.read(&t, 1);
			alf[(uint8_t)t].freq += 1.0;
			size++;
		}
		for (uint16_t i = 0; i != 256; i++) {
			alf[i].freq /= size;
		}
		std::fstream output("task2_report.txt", std::ios::trunc | std::ios::out);
		output << "Table of frequencies\n";
		for (uint16_t i = 0; i != 256; i++) {
			output << alf[i];
		}
		input.close();
		output.close();
	}
	
}

void task3()
{
	uint32_t mode;
	std::cout << "Choose mode:\n 1. Encrypt\n 2. Decipher\n";
	std::cin >> mode;
	if (std::cin.good()) {
		switch (mode)
		{
		case 1:
			if (!encrypt()) {
				std::cout << "Error while encrypting\n";
			}
			break;
		case 2:
			decipher();
			break;
		default:
			std::cout << "Incorrect input\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return task3();
			break;
		}
	}
	else {
		std::cout << "Incorrect input\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return task3();
	}
}

uint64_t num_of_bytes(const std::string& filename) {
	uint64_t sum = 0;
	std::fstream input(filename, std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		std::cout << "Error, file is not open\n";
	}
	else {
		std::string text;
		std::getline(input, text);
		sum = text.size();
	}
	input.close();
	return sum;
}

std::string input_filename()
{
	std::string filename;
	std::cout << "Input filename\n";
	std::cin >> filename;
	return filename;
}

bool generate_key()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 255);
	std::vector<int32_t> keys(256);
	int32_t y;

	for (uint16_t i = 0; i != 256; i++) {
		keys[i] = -1;
	}

	for (uint16_t i = 0; i != 256; i++) {
		while (check_keys(keys, y = dist(gen))) {};
		keys[i] = y;
	}
	
	std::fstream output("keys.txt", std::ios::out | std::ios::trunc);
	if (!output.is_open()) {
		std::cout << "keys.txt has not been opened\n";
		return false;
	}
	else {
		for (uint16_t i = 0; i != 256; i++) {
			output << i << ' ';
		}
		output << '\n';
		for (uint16_t i = 0; i != 256; i++) {
			output << keys[i] << ' ';
		}
		output << '\n';
	}
	return true;
}

bool check_keys(std::vector<int32_t>& vec, const int32_t x)
{
	for (std::vector<int32_t>::iterator it = vec.begin(); it != vec.end(); it++) {
		if (x == *it) {
			return true;
		}

		if (*it == -1) {
			return false;
		}
	}
	return false;
}

bool encrypt()
{
	std::string filename = input_filename();
	if (!generate_key()) {
		return false;
	}
	std::cout << "Keys have been generated\n";
	std::vector<Positions> keys = read_keys(false);
	return change_content(keys, filename, "e_");
}

bool decipher()
{
	std::string filename = input_filename();
	std::vector<Positions> keys = read_keys(true);
	return change_content(keys, filename, "d_");
}

std::vector<Positions> read_keys(bool invert)
{
	std::fstream keys("keys.txt", std::ios::in);
	std::vector<Positions> pos(256);
	std::string temp;
	for (uint16_t i = 0; i != 256; i++) {
		keys >> temp;
		pos[i].pos1 = (uint8_t)std::stoi(temp);
	}
	for (uint16_t i = 0; i != 256; i++) {
		keys >> temp;
		pos[i].pos2 = (uint8_t)std::stoi(temp);
	}
	if (invert) {
		uint8_t t;
		for (uint16_t i = 0; i != 256; i++) {
			t = pos[i].pos1;
			pos[i].pos1 = pos[i].pos2;
			pos[i].pos2 = t;
		}
		std::sort(pos.begin(), pos.end(), [](Positions a, Positions b) {
			return a.pos1 < b.pos1;
			});
	}
	return pos;
}

bool change_content(const std::vector<Positions>& pos, const std::string& filename, const std::string& prefix)
{
	std::fstream input(filename, std::ios::in | std::ios::binary);
	std::fstream output(prefix + filename, std::ios::out | std::ios::trunc);
	if (!output.is_open()) {
		return false;
	}
	char t;
	while (input.read(&t, 1)) {
		output << pos[(uint8_t)t].pos2;
	}
	return true;
}
