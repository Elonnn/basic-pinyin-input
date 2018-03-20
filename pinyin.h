#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_map>
#include<set>
#include <algorithm>
#include<iomanip>
#include<cassert>
//git
using namespace std;

class info
{
public:
	vector<string> candidate_word_table;
	info() {}
	info(string s)
	{
		assert(s.length() % 2 == 0);
		while (!s.empty())
		{
			candidate_word_table.push_back(s.substr(0, 2));
			s.erase(0, 2);
		}
	}
};

typedef unordered_map<string, info> hmap;

hmap syllabary;
set<string> word_book;
set<string> user_word_book;
string sentence;	//���ڽ���������ı�

void read_file();			//���ʿ� ���ڱ� �û��ʿ�
void syllable_partition(string input_s, vector<string> &syl_seq, vector<hmap::iterator> &syl_it);//�������ƥ��ԭ�� ����û����������
int choose(vector<string> &Candidate);	//���ݺ�ѡ�ʱ� �õ��û�ѡ�� ����Ϊ��ѡ�ʱ� �����û�ѡ��Ĵ�
void match_word(int i, vector<string> &syl_seq, vector<hmap::iterator> &syl_it, vector<string> &Candidate, set<string> &word_book);//����ƴ�����кʹʱ� �жϵ�ǰƥ��λ�������ܹ��ɵĴ� �������ѡ�ʱ�
void save();				//ѡ���Ƿ� �����û��ʿ� ������������