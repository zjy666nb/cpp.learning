#include <iostream>
#include <memory>
using namespace std;
class zhinengzhizheng1 {
private:
	unique_ptr<int[] > date;
	int size;
public:
	zhinengzhizheng1(int s) :size(s), date(make_unique<int[]>(s)) {
		for (int i = 0;i < size;i++) {
			date[i] = 0;
		}
	}
	void set(int index, int value) {
		if (index >= 0 && index < size)
			date[index] = value;
	}
	void print() {
		for (int i = 0;i <= size;i++)
			cout << date[i] << " ";
	}
	void resize(int newsize) {
		auto newdata = make_unique<int[]>(newsize);
		for (int i = 0;i < (newsize < size ? newsize : size);i++) {
			newdata[i] = date[i];
		}
		for (int i = size;i < newsize;i++) {
			newdata[i] = 0;
		}
		date = move(newdata);
		size = newsize;
	}


};
int main() {
	zhinengzhizheng1 arr(5);
	arr.set(0, 10);
	arr.set(2, 30);
	arr.print();
	arr.resize(6);
	arr.print();
	return 0;
}