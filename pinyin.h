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
string sentence;	//用于接受输入的文本

void read_file();			//读词库 音节表 用户词库
void syllable_partition(string input_s, vector<string> &syl_seq, vector<hmap::iterator> &syl_it);//根据最大匹配原则 拆分用户输入的音节
int choose(vector<string> &Candidate);	//根据候选词表 得到用户选择 参数为候选词表 返回用户选择的词
void match_word(int i, vector<string> &syl_seq, vector<hmap::iterator> &syl_it, vector<string> &Candidate, set<string> &word_book);//根据拼音序列和词表 判断当前匹配位置向后可能构成的词 并加入候选词表
void save();				//选择是否 保存用户词库 保存输入内容