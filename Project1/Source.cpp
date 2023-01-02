#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;
class Single
{
protected:
	Single(const string val) : val_(val) {}
	static std::mutex mutex_;
	static Single* single_;
	std::string val_;
public:
	Single(const Single&) = delete;
	void operator=(const Single&) = delete;
	static Single* getI(const string&); // get instance
	string getVal() const {
		return val_;
	}
};
class Foo {
	std::string name;
public:
	Foo(const char* name) : name(name) {}
	void operator()() {
		this_thread::sleep_for(chrono::milliseconds(500));
		Single* single = Single::getI(name);
		cout << single->getVal().c_str() << "\n";
	}
};
std::mutex Single::mutex_;
Single* Single::single_ = nullptr;
Single* Single::getI(const string& val)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (single_ == nullptr) {
		single_ = new Single(val);
	}
	return single_;
}
void doFirst() {
	this_thread::sleep_for(chrono::milliseconds(1000));
	Single* single = Single::getI("First");
	cout << single->getVal().c_str() << "\n";
}
int doSecond(unsigned int n) {
	this_thread::sleep_for(chrono::milliseconds(1000));
	Single* single = Single::getI("Second");
	cout << single->getVal().c_str() << "\n";
	n = thread::hardware_concurrency();
	return n;
}
int main()
{
	int tcount;
	Foo f1("first");
	Foo f2("second");
	thread t1;
	thread t2;
	cout << "Joinable 1 thread before joining: " << t1.joinable() << "\nJoinable 2 thread before joining: " << t2.joinable() << endl;
	t1 = thread(doFirst);
	t2 = thread([&tcount]() {tcount = doSecond(0); });
	cout << "Joinable 1 thread after joining: " << t1.joinable() << "\nJoinable 2 thread after joining: " << t2.joinable() << endl;
	cout << "ID1 = " << t1.get_id() << "\tID2 = " << t2.get_id() << endl;
	std::swap(t1, t2);
	cout << "ID1 = " << t1.get_id() << "\tID2 = " << t2.get_id() << endl;
	t1.detach();
	t2.join();
	cout << "Count of flows = " << tcount << endl;
	/*
	Foo f1("first");
	Foo f2("second");
	thread t1(f1);
	thread t2(f2);
	if (t1.joinable())
		t1.join();
	if (t2.joinable())
		t2.join();
	*/
	return 0;
}