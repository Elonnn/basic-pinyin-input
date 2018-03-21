#include"pinyin.h"

int main()
{
	//读词库 音节表 用户词库
	read_file();
	cout << "Welcome to pinyin input method! Now you can input your pinyin sequence or input Q to quit.\nTips to know:\n\
1.Your input must be complete spelling, which means you cannot omit any necessary letters.\n\
2.The letter v is not accepted. For example, to spell 许, \"xv\" should be converted to \"xu\".\n\
3.For ambiguous pinyin sequence, you can add ' to avoid ambiguity. Otherwise, the rule of maximum match will be applied. \
For example, to spell 李安, input li'an to distinguish from 连.\n";
	while (1)
	{
		string input_s;
		cin >> input_s;	//不会接受空串 不是getline
		if (input_s == "Q")
			break;

		//根据最大匹配原则 拆分用户输入的音节
		vector<string> syl_seq;
		vector<hmap::iterator> syl_it;//后面还要用 先记录下来
		try {
			syllable_partition(input_s, syl_seq, syl_it);
		}
		catch (string wrong_seq)
		{
			cerr << "Unrecognized pinyin sequence is encountered:" << wrong_seq << "!\n\
Please re-input:\n";
			continue;	//回大循环 重头再来  不用goto就要加个大循环
		}

		//根据拆分得到的音节一次次给出候选 直到音节全部成词
		string word_group;	//当前输入的这一词组
		for (int i = 0, len = 1; i < syl_seq.size(); i += len)	//也可以把前面处理过的串都删掉 更“递归” 少一个变量更容易想
		{
			//先输出一下已匹配到哪了
			cout << sentence << word_group;
			for (int j = i; j < (int)syl_seq.size() - 1; j++)	//设计成坑爹无符号数 通常不会有问题 但万一0-1就有问题了 保险起见
				cout << syl_seq[j] << "'";
			if (syl_seq.size() >= 1)
				cout << syl_seq[syl_seq.size() - 1] << endl;

			vector<string> Candidate;//这一轮要给出的一大串候选 包括词 和 字
									 //hmap::iterator p_syl = syllabary.find(syl_seq[]);	其实之前找过 所以为了效率 记录下来 空间换时间
			vector<string> &cand = syl_it[i]->second.candidate_word_table;	//第i个音节的字表

																			//判断词
			match_word(i, syl_seq, syl_it, Candidate, user_word_book);
			match_word(i, syl_seq, syl_it, Candidate, word_book);

			//可能的词已经列完了 下面只能按字频给出字
			Candidate.insert(Candidate.end(), cand.begin(), cand.end());

			int n = choose(Candidate);
			word_group += Candidate[n - 1];
			len = Candidate[n - 1].length() / 2;
		}
		sentence += word_group + " ";
		cout << "This time you've typed in " << word_group << " .\nYou can continue to type in or input Q to quit and save.\n" << sentence << endl;

		//加入用户词典
		user_word_book.insert(word_group);	//没有问题 添加重复元素自动失败
		if (user_word_book.find(word_group.substr(0, 2)) == user_word_book.end())	//对于一字 先加过了 就不会重复加
			user_word_book.insert(word_group.substr(0, 2));
	}

	//选择是否 保存用户词库 保存输入内容
	save();

	cout << "If you are satisfied with our product, please give 5 scores as an encouragement. Thank you very much!\n";
	getchar();

	return 0;
}


//读词库 音节表 用户词库
void read_file()
{
	cout << "Please copy resource file to the directory of this project.\nPress Enter to continue.\n";
	getchar();
	ifstream F_pinyin("pinyin.txt");
	ifstream F_word("word.txt");
	try {
		if (F_word.fail())
			throw "word.txt";
		if (F_pinyin.fail())
			throw "pinyin.txt";
	}
	catch (string file) {
		cout << "Fail to open " << file << "!\n";
		getchar();
		assert(0);
	}
	cout << "Loading...\n";

	do {
		string syl, temp;
		getline(F_pinyin, syl, ',');
		F_pinyin >> temp;
		info table(temp);
		syllabary[syl] = table;
		F_pinyin.get();	//把最后的'\n'读掉 否则会在下一个syl里面
	} while (F_pinyin.good());
	F_pinyin.close();

	string temp;
	while (F_word >> temp)
	{
		word_book.insert(temp);
		if (word_book.find(temp.substr(0, 2)) == word_book.end())	//对于一字 先加过了 就不会重复加
			word_book.insert(temp.substr(0, 2));
	}
	F_word.close();

	while (true)
	{
		cout << "Do you need to load your user word book from user_word.txt? (y/n)\n";
		string answer;
		cin >> answer;
		if (answer == "y")
		{
			ifstream F_user_word("user_word.txt");	//用户词库
			if (F_user_word.fail())
			{
				cerr << "Fail to open the user_word file! Please check if the file does exist.\n";
				getchar();
				continue;
			}
			string temp;
			while (F_user_word >> temp)
			{
				user_word_book.insert(temp);
				if (user_word_book.find(temp.substr(0, 2)) == user_word_book.end())	//对于一字 先加过了 就不会重复加
					user_word_book.insert(temp.substr(0, 2));
			}
			F_user_word.close();
			cout << "Your user word book has been successfully loaded!\n";
			break;
		}
		else if (answer == "n")
			break;
		else
			cerr << "Invalid input!\n";
	}

	cout << "OK, data entry is completed!\n";
}

//根据最大匹配原则 拆分用户输入的音节
void syllable_partition(string input_s, vector<string> &syl_seq, vector<hmap::iterator> &syl_it)
{
	while (!input_s.empty())
	{
		if (input_s.at(0) == '\'')//每次新的待处理的串 都会先判断并删除开始的一个'
			input_s.erase(0, 1);
		int test_len = 6 < input_s.length() ? 6 : input_s.length();
		int i = test_len;
		for (; i > 0; i--)
		{
			hmap::iterator it = syllabary.find(input_s.substr(0, i));
			if (it != syllabary.end())
			{
				syl_seq.push_back(input_s.substr(0, i));
				syl_it.push_back(it);
				input_s.erase(0, i);
				break;		//识别成功 继续处理后面的音节
			}
		}
		if (i == 0)	//i=从test_len到0 都不匹配（没有中途break出来） 则无法识别
			throw input_s.substr(0, test_len);
	}
}

//根据候选词表 得到用户选择
int choose(vector<string> &Candidate)	//参数为候选词表 返回用户选择的词
{
	//输出候选词表
	cout << setiosflags(ios::left);
	for (int i = 0; i < Candidate.size(); i++)
	{
		if (i % 4 == 0)
			cout << endl;
		cout << i + 1 << ". " << Candidate[i] << "\t\t";
	}

	//得到用户的选择
	int n;
	while (1)
	{
		try {
			cout << "\nYour choice is:";
			cin >> n;
			if (cin.fail())
			{
				cin.clear();
				string buff;
				getline(cin, buff);
				throw buff;
			}
			if (n < 1 || n - 1 >= Candidate.size())
				throw n;
			break;
		}
		catch (string s)
		{
			cout << "Invalid input! " << s << " is not a number!\n";
		}
		catch (int n)
		{
			cout << "Illegal input! " << n << " have crossed the border!\n";
		}
	}

	return n;
}

//根据拼音序列和词表 判断当前匹配位置向后可能构成的词 并加入候选词表
void match_word(int i, vector<string> &syl_seq, vector<hmap::iterator> &syl_it, vector<string> &Candidate, set<string> &word_book)
{
	vector<string> &cand = syl_it[i]->second.candidate_word_table;	//第i个音节的字表
	if (i < (int)syl_seq.size() - 1)  //加上对只剩一个字的情况执行效率更高一点 
									  //判断从这个音节开始 是否可能成词
		for (int j = 0; j < cand.size(); j++)//在字表里面逐个尝试 e.g.蜗居 莴苣...
		{
			set<string>::iterator it = word_book.find(cand[j]);//先要能找到字 找不到直接可认为没有 之前已经把词的首字全加进set了 根据首字来查词
			if (it == word_book.end())
				continue;
			set<string>::iterator wd_it = it;
			wd_it++;			//看这个字后面都跟了哪些词
			for (; wd_it != word_book.end() && wd_it->substr(0, 2) == (*it); wd_it++)	//对于wd_it这个词 检验后面的音节是否可以与之匹配		e.g.我是 我市 我使得 假使这些都是词
			{
				if (wd_it->length() / 2 - 1 + i >= syl_seq.size())	//加上wd_it的词长 取二字词试试看就理解了  直接越界了 肯定没戏 看下个词吧
					continue;
				int k = 1;
				for (; k < wd_it->length() / 2; k++)	//第k个字是否匹配 第0个已经确定匹配了 不用判断
				{
					vector<string> &next_cand = syl_it[i + k]->second.candidate_word_table;		//看下一个音节的词表 里有没有这个字
					if (find(next_cand.begin(), next_cand.end(), wd_it->substr(2 * k, 2)) == next_cand.end()) //没找到
						break;
				}
				if (k == wd_it->length() / 2)	//一直匹配到了最后
				{
					if (find(Candidate.begin(), Candidate.end(), *wd_it) == Candidate.end()) //还不在词表里
						Candidate.push_back(*wd_it);
				}
			}
		}
}

//选择是否 保存用户词库 保存输入内容
void save()
{
	while (true)
	{
		cout << "Do you want to save your user word book? (y/n)\n";
		string answer;
		cin >> answer;
		if (answer == "y")
		{
			ofstream F_user_word("user_word.txt");	//用户词库
			if (F_user_word.fail())
			{
				cerr << "Fail to create the user_word file!\n";
				getchar();
				assert(0);
			}
			for (set<string>::iterator it = user_word_book.begin(); it != user_word_book.end(); it++)
				F_user_word << *it << endl;
			F_user_word.close();
			cout << "Your user word book has been successfully saved to user_word.txt\n";
			break;
		}
		else if (answer == "n")
			break;
		else
			cerr << "Invalid input!\n";
	}

	while (true)
	{
		cout << "Do you want to save your input content? (y/n)\n";
		string answer;
		cin >> answer;
		if (answer == "y")
		{
			ofstream F_user_file("user_file.txt");
			if (F_user_file.fail())
			{
				cerr << "Fail to save the file!\n";
				getchar();
				assert(0);
			}
			F_user_file << sentence;
			F_user_file.close();
			cout << "Your input content has been successfully saved to user_file.txt\n";
			break;
		}
		else if (answer == "n")
			break;
		else
			cerr << "Invalid input!\n";
	}
}
