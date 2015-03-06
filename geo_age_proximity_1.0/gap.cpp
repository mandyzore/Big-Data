#include <iostream>
#include <fstream>

#include <ctime>
#include <cstdlib>
#include <cstring>

#include <cmath>
#include <vector>
#include <map>

using namespace std;


const unsigned int NUM_USERS=1e7;
const int MAX_AGE =1;
const int MIN_AGE =100;
const int MAX_LAT =90;
const int MIN_LAT =-90;
const int MAX_LON =180;
const int MIN_LON =-180;
const unsigned char BASE32[]="0123456789bcdefghjkmnpqrstuvwxyz";

const double EARTH_RADIUS = 6378.137; //地球半径
const double PI = 3.1415926;

typedef struct UserStruct {
	int id;
	double lat;
	double lon;
	string name;
	char gh[2];
	int age;
} User;

//------------------------------------------------------Trie
class Node { 
	private: char mContent;//0-z
	//bool mMarker;//leaf?
	vector<Node*> mChildren; //chirld node list.size()<= 32
	vector<User*> users;//leaf: neighbor user list 
	public: Node() 
	{ 
		mContent = ' '; 
		//mMarker = false; 
	}
	
	~Node(){}
	char content() { return mContent; } 
	void setContent(char c) { mContent = c; } 
	//bool wordMarker() { return mMarker; } 
	//void setWordMarker() { mMarker = true; } 
	Node* findChild(char c); 
	void appendChild(Node* child) {
		mChildren.push_back(child); 
	}
	void appendUser(User* user){
		users.push_back(user);
	}
	
	vector<Node*> children() { return mChildren; } 
	vector<User*> getUsers() {return users;}
}; 

//Tire.h
class Trie { 
	public: Trie(); ~Trie(); 
	void addUser(User *u);
	vector<User*> searchUserPrefix(string s); 
	private: Node* root; 
}; 


Node* Node::findChild(char c) {
	for ( int i = 0; i < mChildren.size(); i++ )
	{
		Node* tmp = mChildren.at(i); 
	 	if ( tmp->content() == c ) // compare child node symbol
		{ return tmp; } 
	} 
	return NULL; 
} 

Trie::Trie() { root = new Node(); }
Trie::~Trie() { 
	// Free memory
} 

void Trie::addUser(User *user){
	Node* current = root; 
	string geohash= (*user).gh;
	for(int i=0;i<geohash.size();i++)//6
	{
		Node* child = current->findChild(geohash[i]); 
		if ( child != NULL ) { current = child; }// char node
		else {
			Node* tmp = new Node(); 
			tmp->setContent(geohash[i]); 
			current->appendChild(tmp); 
			current = tmp; 
		}
		if ( i == geohash.size() - 1 ){//leaf
	 		current-> appendUser(user);
			//current->setWordMarker();
			return;
	 	}
	}
}

vector<User*> Trie::searchUserPrefix(string s) 
{ 
	Node* current = root; 
	while ( current != NULL ) 
	{ for ( int i = 0; i < s.size(); i++ ) 
		{ 
			Node* tmp = current->findChild(s[i]); 
			if ( tmp == NULL ) 
				return current->getUsers(); 
			current = tmp; 
		} 
	//if ( current->wordMarker() ) 
		return current->getUsers();  
	}
	return current->getUsers(); 
}


//------------------------------------------------------Geo:Hash
static void encode_geohash(double latitude, double longitude, int precision, char *geohash) 
{  
	int is_even=1, i=0;  
	double lat[2], lon[2], mid;  
	unsigned char bits[] = {16,8,4,2,1};  
	int bit=0, ch=0;  
	lat[0] = -90.0; lat[1] = 90.0;  
	lon[0] = -180.0; lon[1] = 180.0;  
	while (i < precision) {  
		if (is_even) {  
			mid = (lon[0] + lon[1]) / 2;  
			if (longitude > mid) {  
				ch |= bits[bit];  
				lon[0] = mid;  
			} else  
				lon[1] = mid;  
			} else {  
				mid = (lat[0] + lat[1]) / 2;  
				if (latitude > mid) {  
					ch |= bits[bit];  
					lat[0] = mid;  
		       		 } else  
					lat[1] = mid;  
			}  
		is_even = !is_even;  

		if (bit < 4)  
			bit++;  
		else {  
			geohash[i++] = BASE32[ch];  
			//cout<<ch<<endl;
			bit = 0;  
			ch = 0;  
		}  
	}  
	geohash[i] = 0;  
}  

//------------------------------------------------------Geo: Distance
static double getEarthDistance(double latA, double lngA,double latB, double lngB)
{
	double distance;
	latA=90-latA;
	latB=90-latB;
	//distance = 
	distance = EARTH_RADIUS*acos((sin(latA)*sin(latB)*cos(lngA-lngB) + cos(latA)*cos(latB)))*PI/180;
	return distance;
}

//------------------------------------------------------HeapSort
void SwapTwo(double &a, double &b)  
{  
	double temp = a;  
	a = b;  
	b= temp;  
}  

void siftup(double nums[], int i)
{  
	if(i==0) return;  

	int p = (i-1)/2;  

	if (nums[i]>nums[p]) return;// >: minHeap; <:maxHeap

	else  
	{
		SwapTwo(nums[i], nums[p]);  
		siftup(nums, p);  
	}  
}  

void MakeHeap(double nums[], int size)  
{  
	for (int i=0; i<size; i++)           // using siftup function  
		siftup(nums, i);  
}  

void Display(double nums[], int n, int N)  
{  
	//int size =nums.size()-1;
	for (int i=0; i<N; i++)  
	{  
		cout << nums[n-i-1]<<endl;  
	}  
}  

void HeapSort(double nums[], int size)  
{  
	for (int i=size-1; i>=0; i--)  
	{  
		SwapTwo(nums[i], nums[0]);      // step 2, swapping the fisrt node(top) and the last node(leaf).  
		size--;                         // remove the sorted numbers.  
		MakeHeap(nums, size);           // make max-heap for unsorted numbers.  
	}  
}  

//------------------------------------------------------Similar Matrix
vector<User*> getTopNSimilar(int N, User* user,vector<User*> neighbors)
{
	vector<User*> top10;
	int n=neighbors.size();
	double nums[n];
	double numss[n];
	double dis;
	int ageGap;
	int ages[n];
	for(int i=0;i<n;i++){
		dis=getEarthDistance(user->lat,user->lon,neighbors[i]->lat,neighbors[i]->lon);
		ageGap=user->age - neighbors[i]->age;
		if(ageGap<0){
			ageGap=-ageGap;
		}
		nums[i]=dis+ageGap;// distance(km) + age_gap
		numss[i]=nums[i];
	}
	
	MakeHeap(nums, n);  
	HeapSort(nums, n);
	//Display(nums, n, N);
	double top;
	for(int i=0;i<N;i++){
		top=nums[n-i-1];
		for(int j=0;j<n;j++){
			if(numss[j]==top){
				top10.push_back(neighbors[j]);
			}
		}
	}
	return top10;
}


//------------------------------------------------------
//---------------------  Main  -------------------------
//------------------------------------------------------
int main()
{
	clock_t start,end;
	//---------------------------------------------------
	//---0. load name.txt to memory LIST---
	cout<<"step1: load name.txt..."<<endl;
	start = clock(); 
	
	char buffer[14];//max length of name
	string names[21986];//total num of names
	int numOfName=0;
	
	fstream out;
	out.open("name.txt",ios::in);
	while(!out.eof())
	{
		out.getline(buffer,14,'\n');
		if(strlen(buffer)>0){
			names[numOfName]=buffer;
			numOfName+=1;
		}
	}
	out.close();
	//---------------------------------------------------
	//---1. user data generate---(10 million users)------
	cout<<"step2: generate users..."<<endl;
	
	int numOfUser = NUM_USERS;
	int userID = 0;
	double lat,lon;
	int nameID,age;
	string n;
	User* u;
	Trie* trie = new Trie(); 
	while(userID<numOfUser){
		u=new User();
		u->id=userID;
		//----2.1 latitude [-90,90]
		lat = (double) rand()/RAND_MAX * (MAX_LAT-MIN_LAT) + MIN_LAT;
		//----2.2 longitude [-180,180]
		lon = (double) rand()/RAND_MAX * (MAX_LON-MIN_LON) + MIN_LON;
		//----2.3 age: [1,100]
		age = (double) rand()/RAND_MAX * (MAX_AGE-MIN_AGE) + MIN_AGE;
		//----2.4 name [0,21985] the num of simple names in name.txt
		nameID =(double)rand()/RAND_MAX * (numOfName-1);
		
		encode_geohash(lat,lon,2, u->gh);
		u->name = names[nameID];
		u->lat=lat;
		u->lon=lon;
		u->age=age;
		trie->addUser(u);
		
		userID+=1;
	}
	end=clock();
	
	double time= (end - start) / CLOCKS_PER_SEC;
	cout<<"..."<<NUM_USERS<<" users generation time: "<<time<<"s"<<endl;
	//delete names;
	
	//---------------------------------------------------
	//---2. similar user query, command-line interface---
	cout<<"step3: input query user..."<<endl;
	int Qage;
	float Qlat;
	float Qlon;
	cout << "...please enter the query user's age between 1 to 100: ";
	cin >> Qage;
	cout << "...please enter the query user's latitude between -90 to 90: ";
	cin >> Qlat;
	cout << "...please enter the query user's longitude between -180 to 180: ";
	cin >> Qlon;
	cout<<"step2: search top10 similar user  for your query user: location=("<<Qlat<<","<<Qlon<<"),age="<<Qage<<"..."<<endl;
	
	User *user=new User();
	user->age=Qage;
	user->lat=Qlat;
	user->lon=Qlon;
	encode_geohash(Qlat,Qlon,2, user->gh);// two levels deep Trie
	
	start=clock();
	vector<User*> nb;
	string s=user->gh;
	nb=trie->searchUserPrefix(user->gh);
	//cout<<s<<" "<< nb.size() <<endl;
	//delete trie;
		
	end=clock();
	
	//----2.1 top10 similar search 
	vector<User*> top10=getTopNSimilar(10,user,nb);
	for(int i=0;i<10;i++){
		cout<<"top"<<i<<": location=("<< top10[i]->lat <<","<<top10[i]->lon <<"), age="<<top10[i]->age<<", name="<<top10[i]->name<<", userId="<<top10[i]->id<<endl;
	}
	
	cout<<endl;
	time= (start-end) / CLOCKS_PER_SEC;
	cout<<"...search time: "<<time<<"s "<<endl;
	
	return 0;
}
