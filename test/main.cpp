#include <cstdio>
#include <cstring>

class Test{
protected:
	char *mName;
public:
	Test(){
		printf("Test()\n");
		mName = NULL; 
	}
	Test(char *name){
		printf("Test(char *name)\n");
		mName = new char[strlen(name)+1];
		strcpy(mName, name);
	}
	~Test(){
		printf("~Test()\n");
		if (mName != NULL) delete[] mName;
	}
	Test(Test& test){
		printf("Test(Test& test)\n");
		mName = new char[strlen(test.mName)+1];
		strcpy(mName, test.mName);
	}
	void printName(){
		puts(mName);
	}

	void operator=(Test& test){
		printf("operator=(Test& test)\n");
		if (mName != NULL){
			delete[] mName;
			mName = NULL;
		}
		if (test.mName != NULL){
			mName = new char[strlen(test.mName)+1];
			strcpy(mName, test.mName);
		}
	}
};

class Test2 : public Test{
	int test2val;
public:
	Test2() : Test(){ mName = NULL; }
};

void dummyFunc(Test t){
	t.printName();
}

int main(){
	Test t("baba");
	Test2 t2;
	//t = t2;

	return 0;
}