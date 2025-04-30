#include <iostream>
using namespace std;
class Dontaishuzu {
private:
	unique_ptr<int[] > data;
	int size;
public:
	Dontaishuzu(int s) :size(s), data(make_unique<int[]>(s)) {
		for (int i = 0;i < size;i++) {
			data[i] = 0;
		}
	}
	void set(int index, int value) {
		if (index >= 0 && index < size)
			data[index] = value;
	}
	void print() {
		for (int i = 0;i <= size;i++)
			cout << data[i] << " ";
	}
	void resize(int newsize) {
		auto newdata = make_unique<int[]>(newsize);
		for (int i = 0;i < (newsize < size ? newsize : size);i++) {
			newdata[i] = data[i];
		}
		for (int i = size;i < newsize;i++) {
			newdata[i] = 0;
		}
		data = move(newdata);
		size = newsize;
	}
	

};
int main() {
	Dontaishuzu arr(5);
	arr.set(0, 10);
	arr.set(2, 30);
	arr.print();
	arr.resize(6);
	arr.print();
	return 0;
}