#include <iostream>
#include <vector>
using namespace std;
class shape {
public:
	virtual double area() const = 0;
	virtual void draw() const { cout << "Drawing Shape" << endl; }
	  virtual ~shape() {}
};
class cirle :public shape {
private:
	double radius;
public:
	cirle(double r) :radius(r) {}
	double area()const override { return 3.14 * radius * radius; }
	void draw() const override { cout << "radius:" << endl; }
};
class rectangle :public shape {
private:
	double width, height;
public:
	rectangle(double w, double h) :width(w), height(h){}
	double area()const override { return width * height; }
	void draw()const override { cout << "size:" << width << "x" << height << endl; }
};
int main() {
	vector<shape*>shapes;
	shapes.push_back(new cirle(5.0));
	shapes.push_back(new rectangle(4, 6));
	for (auto s : shapes) {
		s->draw();
		cout << "area:" << s->area() << endl;
	}
	for (auto s : shapes)delete s;
}