#include <vector>
using std::vector;

class OneLineRecord{
	public:
	int Click, Impression;
	unsigned long long int DisplayURL;
	int AdID, AdvertiserID;
	int Depth, Position, QueryID;
	int KeywordID, TitleID, DescriptionID;
	int UserID;
	
	public:
	int Line2Record(char* line);
	void print() const;
	void PrintAdProperty();
};
class UserIndex{
	public:
	int UserID;
	int begin, end;
	public:
	UserIndex(){}
	UserIndex(int init_userid, int init_begin, int init_end);
};
class DataListVector{
	public:
	vector<OneLineRecord> RecordList;
	vector<UserIndex> UserList; 
	
	struct property_sort_compare{
		const DataListVector& data;
		property_sort_compare(const DataListVector& input) : data(input){}
		bool operator()(const OneLineRecord* a, const OneLineRecord* b);
	};
	struct record_sort_compare{
		const DataListVector& data;
		record_sort_compare(const DataListVector& input) : data(input){}
		bool operator()(OneLineRecord a, OneLineRecord b); 
	};
	public:
	DataListVector(){}
	DataListVector(unsigned int data_init_size); 

	void AddRecord(const OneLineRecord& r);
	unsigned int RecordSize();

	void SortUAQDPAndBuildUserList();

	void get(int userid, int ad, int query, int pos, int depth);
	int GetFstUAQPDRecord(int userid, int ad, int query, int pos, int depth);
	int GetUserListIndex(int userid);

	void clicked(int userid);

	void impressed(int u1, int u2);

	void profit(int ad, double theta);

	private:
	void PrepareUserList();
	int search_compare(int userid, int ad, int query, int pos, int depth, int index);
	int CompareLessOrMore(int array_in_order1[], int array_in_order2[], int num);
	void PrintCommonAd(int& index1, int end1, int& index2, int end2);
	int AdPropertyCompare(const OneLineRecord& r1, const OneLineRecord& r2);
	void BuildPropertyList(vector<OneLineRecord*>& list, int& index, int end, int ad);
	int GetFstAdIndex(int ad, int user_begin, int user_end);
};


