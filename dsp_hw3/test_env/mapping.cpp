#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

int glob_count = 0;

vector<vector<string> > words;

int getIndexOfZhuyin(string s, int begin) {
	if (s.size() < begin + 2)
		return -1;

	string zhuyin = s.substr(begin, 2);
	for (int i = 0; i < glob_count; ++i) {
		if (zhuyin == words[i].front())
			return i;
	}

	words[glob_count].push_back(zhuyin);
	return glob_count++;
}

bool myfunction (vector<string> v1, vector<string> v2) {
	string s1 = v1[0], s2 = v2[0];

	if ((unsigned char)s1[0] < (unsigned char)s2[0]) return true;
	if ((unsigned char)s1[0] > (unsigned char)s2[0]) return false;

	return (unsigned char)s2[0] < (unsigned char)s1[0];
}

int main() {
	words.resize(37);
	fstream fin ("Big5-ZhuYin.txt", fstream::in | fstream::binary);

	string word, zhuyins;
	while (fin.peek() != -1) {
		fin >> word >> zhuyins;
		fin.get();
		int position = 0, prevPosition = 0;
		int prevIndex = -1;
		do {
			int index = getIndexOfZhuyin(zhuyins, position);
			if (index != -1 && prevIndex != index) {
				words[index].push_back(word);
			}
			prevIndex = index;
			prevPosition = position;
		} while ((position = zhuyins.find_first_of('/', position) + 1) > prevPosition);
	}
	for (int i = 0; i < glob_count; ++i) {
		cout << words[i][0] << " ";
		for (int j = 1; j < words[i].size(); ++j)
			cout << ' ' << words[i][j];
		cout << '\n';
		for (int j = 1; j < words[i].size(); ++j)
			cout << words[i][j] << "  " << words[i][j] << '\n';
	}
	cout << endl;

}

