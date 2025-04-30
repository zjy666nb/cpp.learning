#include<iostream>
#include<string>
using namespace std;
class student {
private:
	string name;
	int id;
	float score;
	

	
public:
	student():name("Unknown"),id(0),score(0.0f){}
	student(string n, int i,float s) :name(n), id(i),score(s) {}
	void setscore(float s) {
		score = s;
	}
	void print()const {
		cout << "NAME:" << name << ",ID:" << id << ",Score:" << score << endl;
	}
};
int main() {
	student s1;
	student s2("ANDJ", 101, 95.5);
	s1.print();
	s2.print();
	s1.setscore(9.99);
	s1.print();
	const student s3("asd", 111, 2.2);
	s3.print();
	return 0;
}