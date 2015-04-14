#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string>
#include "data_list.h"

using std::vector;

//=============================================class OneLineRecord================================================

int OneLineRecord::Line2Record(char* line){
	if(line == NULL)
		return 0;
	if(sscanf(line, "%d %d %llu %d %d %d %d %d %d %d %d %d",
	          &Click, &Impression, &DisplayURL, &AdID, &AdvertiserID, &Depth,
	          &Position, &QueryID, &KeywordID, &TitleID, &DescriptionID, &UserID) != 12) 
		return 0;
	return 1;
}
void OneLineRecord::print() const{
	printf("%5d %6d %20llu %8d %5d %2d %2d %8d %7d %7d %7d %8d\n",
	       Click, Impression, DisplayURL, AdID, AdvertiserID, Depth,
	       Position, QueryID, KeywordID, TitleID, DescriptionID, UserID);
}
void OneLineRecord::PrintAdProperty(){
	printf("%llu %d %d %d %d\n", DisplayURL, AdvertiserID, KeywordID, TitleID, DescriptionID);
}

//==============================================class UserIndex=========================================

UserIndex::UserIndex(int init_userid, int init_begin, int init_end){
	UserID = init_userid;
	begin = init_begin;
	end = init_end;
}
//===========================================class DataListVector===============================================

DataListVector::DataListVector(unsigned int init_size){
	RecordList.reserve(init_size);
}

void DataListVector::AddRecord(const OneLineRecord& r){
	RecordList.push_back(r);
}
unsigned int DataListVector::RecordSize(){
	return RecordList.size();
}
int DataListVector::CompareLessOrMore(int array_in_order1[], int array_in_order2[], int num){
	for(int i=0; i<num; i++){
		if(array_in_order1[i] > array_in_order2[i])
			return 1;
		if(array_in_order1[i] < array_in_order2[i])
			return -1;
	}
	return 0;
}
int DataListVector::GetUserListIndex(int userid){
	unsigned int upper_bound = UserList.size(), lower_bound = 0, index = upper_bound/2;
	int less_or_more, notfound = 0;

	while((less_or_more = CompareLessOrMore(&userid, &(UserList[index].UserID), 1)) != 0 && notfound != 1){
		if(less_or_more > 0){
			if(index == upper_bound-1)
				notfound = 1;
			else{
				lower_bound = index+1;
				index = (index+upper_bound)/2;
			}
				
		}else{
			if(index == lower_bound)
				notfound = 1;
			else{
				upper_bound = index;
				index = (index+lower_bound)/2;
			}
		}
	}
	if(notfound){
		return -1;
	}
	return index;
}

//-------------------------------------------sort----------------------------------------------

bool DataListVector::record_sort_compare::operator()(OneLineRecord a, OneLineRecord b){
	if(a.UserID != b.UserID)
		return (a.UserID < b.UserID);
	if(a.AdID != b.AdID)
		return (a.AdID < b.AdID);
	if(a.QueryID != b.QueryID)
		return (a.QueryID < b.QueryID);
	if(a.Position != b.Position)
		return (a.Position < b.Position);
	if(a.Depth != b.Depth)
		return (a.Depth < b.Depth);

	return false;
}

void DataListVector::SortUAQDPAndBuildUserList(){
	std::sort(RecordList.begin(), RecordList.end(), record_sort_compare(*this));
	PrepareUserList();
}

//--------------------------------------------prepare UserList----------------------------------

void DataListVector::PrepareUserList(){
	int begin = 0;
	int cur_user = RecordList[0].UserID;
	for(unsigned int i=1; i<RecordList.size(); i++){
		if(cur_user != RecordList[i].UserID){
			UserList.push_back(UserIndex(cur_user, begin, i-1));
			cur_user = RecordList[i].UserID;
			begin = i;
		}
	}
	UserList.push_back(UserIndex(cur_user, begin, RecordList.size()-1));
}
//-------------------------------------------get(u,a,q,d,p)----------------------------------------------

void DataListVector::get(int userid, int ad, int query, int pos, int depth){
	int index = GetFstUAQPDRecord(userid, ad, query, pos, depth);
	if(index == -1){
		printf("0 0\n");
		return;
	}
	
	int cmp_array[5], done = 0;
	int input_array[5] = {userid, ad, query, pos, depth};
	unsigned int click = 0, impression = 0;
	do{
		click += RecordList[index].Click;
		impression += RecordList[index].Impression;
		index++;

		if(((unsigned int)index) < RecordList.size()){
			cmp_array[0] = RecordList[index].UserID; 
			cmp_array[1] = RecordList[index].AdID;
			cmp_array[2] = RecordList[index].QueryID;
			cmp_array[3] = RecordList[index].Position;
			cmp_array[4] = RecordList[index].Depth;
			done = CompareLessOrMore(cmp_array, input_array, 5);
		}else
			done = 1;
	}while(!done);

	printf("%u %u\n", click, impression);
}
int DataListVector::search_compare(int userid, int ad, int query, int pos, int depth, int index){
	
	int input_for_cmp[5] = {userid, ad, query, pos, depth};
	int data_for_cmp[5] = {RecordList[index].UserID, RecordList[index].AdID, RecordList[index].QueryID, 
	                       RecordList[index].Position, RecordList[index].Depth};

	int cmp = CompareLessOrMore(input_for_cmp, data_for_cmp, 5);
	if(cmp != 0)
		return cmp;

	if(index == 0)
		return 0;

	int last_for_cmp[5] = {RecordList[index-1].UserID, RecordList[index-1].AdID, RecordList[index-1].QueryID, 
	                       RecordList[index-1].Position, RecordList[index-1].Depth};
	if(CompareLessOrMore(input_for_cmp, last_for_cmp, 5) != 0)
		return 0;

	return -1;
}
int DataListVector::GetFstUAQPDRecord(int userid, int ad, int query, int pos, int depth){
	unsigned int upper_bound = RecordList.size(), lower_bound = 0, index = upper_bound/2;
	int less_or_more, notfound = 0;

	while((less_or_more = search_compare(userid, ad, query, pos, depth, index)) != 0 && notfound != 1){
		if(less_or_more > 0){
			if(index == upper_bound-1)
				notfound = 1;
			else{
				lower_bound = index+1;
				index = (index+upper_bound)/2;
			}
				
		}else{
			if(index == lower_bound)
				notfound = 1;
			else{
				upper_bound = index;
				index = (index+lower_bound)/2;
			}
		}
	}
	if(notfound){
		return -1;
	}
	return index;
}
//-----------------------------------------------clicked()--------------------------------------------

void DataListVector::clicked(int userid){
	int tmp_index = GetUserListIndex(userid);
	if(tmp_index < 0)
		return;

	int begin_i = UserList[tmp_index].begin, end_i = UserList[tmp_index].end;
	int printed_ad_query[2] = {-2,-2}, cur_ad_query[2]={-1,-1};
	bool printed = false;

	for(int i=begin_i; i<=end_i; i++){
		if(printed){
			cur_ad_query[0] = RecordList[i].AdID, cur_ad_query[1] = RecordList[i].QueryID;
			if(CompareLessOrMore(cur_ad_query, printed_ad_query, 2) != 0)
				printed = false;
		}
		if((!printed) && (RecordList[i].Click > 0)){
			printf("%d %d\n", RecordList[i].AdID, RecordList[i].QueryID);
			printed = true;
			printed_ad_query[0] = RecordList[i].AdID, printed_ad_query[1] = RecordList[i].QueryID;
		}
	}
}

//--------------------------------------impressed(u1, u2)------------------------------------

void DataListVector::impressed(int u1, int u2){
	int tmp_index = GetUserListIndex(u1);
	if(tmp_index < 0)
		return;
	int index1 = UserList[tmp_index].begin, end1 = UserList[tmp_index].end;

	tmp_index = GetUserListIndex(u2);
	if(tmp_index < 0)
		return;
	int index2 = UserList[tmp_index].begin, end2 = UserList[tmp_index].end;

	while(index1<=end1 && index2<=end2){
		if(RecordList[index1].AdID < RecordList[index2].AdID){
			index1++;
			continue;
		}
		if(RecordList[index1].AdID > RecordList[index2].AdID){
			index2++;
			continue;
		}

		PrintCommonAd(index1, end1, index2, end2); //add index at the same time
	}
		
}
bool DataListVector::property_sort_compare::operator()(const OneLineRecord* a, const OneLineRecord* b){
	if((*a).DisplayURL != (*b).DisplayURL)
		return ((*a).DisplayURL < (*b).DisplayURL);
	if((*a).AdvertiserID != (*b).AdvertiserID)
		return ((*a).AdvertiserID < (*b).AdvertiserID);
	if((*a).KeywordID != (*b).KeywordID)
		return ((*a).KeywordID < (*b).KeywordID);
	if((*a).TitleID != (*b).TitleID)
		return ((*a).TitleID < (*b).TitleID);
	if((*a).DescriptionID != (*b).DescriptionID)
		return ((*a).DescriptionID < (*b).DescriptionID);

	return false;
}
void DataListVector::BuildPropertyList(vector<OneLineRecord*>& list, int& index, int end, int ad){
	while(RecordList[index].AdID == ad && index <= end){
		list.push_back(&(RecordList[index]));
		index++;
	}
	std::sort(list.begin(), list.end(), property_sort_compare(*this));
}
void DataListVector::PrintCommonAd(int& index1, int end1, int& index2, int end2){
	vector<OneLineRecord*> property_list1, property_list2;
	int ad = RecordList[index1].AdID;
	int cmp=0;
	OneLineRecord *tmp;

	BuildPropertyList(property_list1, index1, end1, ad);
	BuildPropertyList(property_list2, index2, end2, ad);
	int i1=0, i2=0, e1=property_list1.size(), e2=property_list2.size();
	printf("%d\n",ad);
	while(i1 < e1 || i2 < e2){
		if(i1 < e1 && i2 < e2)
			cmp = AdPropertyCompare(*(property_list1[i1]), *(property_list2[i2]));
		if((i1<e1 && cmp<0) || i2>=e2){
			printf("\t");
			property_list1[i1]->PrintAdProperty();
			tmp = property_list1[i1];
			do{
				i1++;
			}while(i1 < e1 && AdPropertyCompare(*(property_list1[i1]), (*tmp)) == 0);
			continue;
		}
		if((i2<e2 && cmp > 0) || i1>=e1){
			printf("\t");
			property_list2[i2]->PrintAdProperty();

			tmp = property_list2[i2];
			do{
				i2++;
			}while(i2 < e2 && AdPropertyCompare(*(property_list2[i2]), (*tmp)) == 0);

			continue;
		}

		printf("\t");
		property_list1[i1]->PrintAdProperty();

		tmp = property_list1[i1];
		do{
			i1++;
		}while(i1 < e1 && AdPropertyCompare(*(property_list1[i1]), *tmp) == 0);
		do{
			i2++;
		}while(i2 < e2 && AdPropertyCompare(*(property_list2[i2]), *tmp) == 0);
	}
}

int DataListVector::AdPropertyCompare(const OneLineRecord& r1, const OneLineRecord& r2){
	if(r1.DisplayURL > r2.DisplayURL)
		return 1;
	if(r1.DisplayURL < r2.DisplayURL)
		return -1;

	int arr1[4] = {r1.AdvertiserID, r1.KeywordID, r1.TitleID, r1.DescriptionID};
	int arr2[4] = {r2.AdvertiserID, r2.KeywordID, r2.TitleID, r2.DescriptionID};

	return CompareLessOrMore(arr1, arr2, 4);

}
//------------------------------------profit()-------------------------------------

void DataListVector::profit(int ad, double theta){
	int index;
	int click_sum, impression_sum;
	for(vector<UserIndex>::iterator i = UserList.begin(); i != UserList.end(); ++i){
		click_sum = 0, impression_sum = 0;
		index = GetFstAdIndex(ad, i->begin, i->end);
		if(index == -1)
			continue;
		while(index <= i->end && RecordList[index].AdID == ad){
			click_sum += RecordList[index].Click;
			impression_sum += RecordList[index].Impression;
			index++;
		}
		if(impression_sum == 0){
			if(click_sum == 0 && theta == 0)
				printf("%d\n", i->UserID);
			continue;
		}
		if(((double)click_sum)/impression_sum >= theta)
			printf("%d\n", i->UserID);

	}

}
int DataListVector::GetFstAdIndex(int ad, int user_begin, int user_end){
	int upper_bound = user_end+1, lower_bound = user_begin, index = (upper_bound+lower_bound)/2; 
	int notfound = 0;

	while((RecordList[index].AdID != ad || (index != user_begin && RecordList[index-1].AdID == ad)) && notfound != 1){
		if(RecordList[index].AdID < ad){
			if(index == upper_bound-1)
				notfound = 1;
			else{
				lower_bound = index+1;
				index = (index+upper_bound)/2;
			}
				
		}else{
			if(index == lower_bound)
				notfound = 1;
			else{
				upper_bound = index;
				index = (index+lower_bound)/2;
			}
		}
	}
	if(notfound)
		return -1;
	
	return index;
}

