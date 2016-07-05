#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
using namespace std;

// 멤버십 함수 inferface
// 모든 멤버십 함수는 이 인터페이스를 상속받는다.
class MembershipFunction{
public:
	virtual double getValue(double x) = 0; // y = f(x)
};

// 삼각형모양 멤버십 함수
class TriangleMemberFunction : public MembershipFunction{
private:
	double mWidth; // 너비
	double mCenter; // 중심점 좌표
	double mPeak; // 최고점 함수값
public:
	TriangleMemberFunction(double width, double center, double peak){
		mWidth = width;
		mCenter = center;
		mPeak = peak;
	}

	virtual double getValue(double x){
		double result;
		if (x <= mCenter)
			result = (2*mPeak / mWidth) * (x - mCenter) + mPeak;
		else
			result = -(2*mPeak / mWidth) * (x - mCenter) + mPeak;

		return (result > 0) ? result : 0;
	}
};

class BellMemberFunction : public MembershipFunction{
private:
	double mWidth; // 너비
	double mCenter; // 중심점 좌표
	double mPeak; // 최고점 함수값
public:
	BellMemberFunction(double width, double center, double peak){
		mWidth = width;
		mCenter = center;
		mPeak = peak;
	}

	virtual double getValue(double x){
		double result = mPeak * (1 - pow(x - mCenter, 2)/pow(mWidth/2, 2)) * exp(-pow(x-mCenter, 2)/pow(mWidth/2, 2));
		return result > 0 ? result : 0;
	}
};

// FuzzySet을 정의한다.
// 예를 들어, if X is A1 and Y is B2, then W is C1 이라는 규칙이 있다면
// A1, B2, C1이 FuzzySet에 해당된다.
// 각 FuzzySet은 멤버십 함수를 갖는다.
class FuzzySet{
private:
	MembershipFunction *mMemberFunc; // 멤버십 함수
public:
	FuzzySet(){
		mMemberFunc = NULL;
	}

	// 멤버십 함수를 등록한다.
	void setMemberFunction(MembershipFunction* memberFunc){
		mMemberFunc = memberFunc;
	};

	// 멤버십 함수의 결과를 반환. y = f(x)
	double getValue(double x){
		if (mMemberFunc != NULL)
			return mMemberFunc->getValue(x);
		else{
			printf("error: Member Function is not assigned\n");
			return 0;
		}
	}
};

// 규칙을 정의한다.
// 예를 들어, if X is A1 and Y is B2, then W is C1 이라는 문장이 규칙이다.
// 이 규칙에서 조건의 수는 A1, B2 총 2개이므로 mCondSize == 2 된다.
class Rule{
private:
	int mCondSize; // 조건이되는 FuzzySet들을 담는 배열의 크기
	int mFuzzysetArrCount; // 현재 등록된 조건 FuzzySet의 수. 최대로 mCondSize까지 커질 수 있음
	FuzzySet* mFuzzysetArr; // 조건이되는 FuzzySet들을 담는 배열
	FuzzySet mFuzzysetResult; // C1이라는 FuzzySet이 결과로 나타난다면 C1을 저장함
	string mFuzzysetResultKey; // C1이라는 FuzzySet이 결과로 나타난다면 문자열 "C1"을 저장함
public:
	Rule(){
		mCondSize = 0;
		mFuzzysetArrCount = 0;
		mFuzzysetArr = NULL;
	}
	Rule(int condSize){
		mCondSize = condSize;
		mFuzzysetArrCount = 0;
		mFuzzysetArr = new FuzzySet[condSize];
	}
	Rule(Rule& rule){
		copyObject(rule);
	}
	~Rule(){
		if (mFuzzysetArr != NULL) delete[] mFuzzysetArr;
	}

	void operator=(Rule& rule){
		if (mFuzzysetArr != NULL){
			delete[] mFuzzysetArr;
			mFuzzysetArr = NULL;
		}
		copyObject(rule);
	}

	// 객체를 복사하는 함수. 복사생성자와 =연산에서 사용됨
	void copyObject(Rule& rule){
		mCondSize = rule.mCondSize;
		mFuzzysetArrCount = rule.mFuzzysetArrCount;
		mFuzzysetResult = rule.mFuzzysetResult;
		mFuzzysetResultKey = rule.mFuzzysetResultKey;
		if (rule.mFuzzysetArr != NULL){
			mFuzzysetArr = new FuzzySet[rule.mCondSize];
			for (int i = 0; i < rule.mCondSize; i++)
				mFuzzysetArr[i] = rule.mFuzzysetArr[i];
		}
		else mFuzzysetArr = NULL;
	}

	// 조건이 되는 FuzzySet 등록
	void addCondFuzzySet(FuzzySet fset){
		if (mFuzzysetArrCount == mCondSize)
			printf("error: FuzzySet Array is full\n");
		else{
			mFuzzysetArr[mFuzzysetArrCount] = fset;
			mFuzzysetArrCount++;
		}
	}

	// 조건이 되는 FuzzySet 배열에서, 특정 index의 FuzzySet을 반환함
	FuzzySet* getCondFuzzySet(int index){
		if (mFuzzysetArr != NULL && mFuzzysetArrCount > index)
			return &mFuzzysetArr[index];
		else{
			printf("error: getCondFuzzySet() failed!\n");
			return NULL;
		}
	}

	// mCondSize 반환
	int getCondSize(){
		return mCondSize;
	}

	// mFuzzysetResultKey 반환
	string getFuzzySetResultKey(){
		return mFuzzysetResultKey;
	}
	
	// mFuzzysetResult 반환
	FuzzySet* getFuzzysetResult(){
		return &mFuzzysetResult;
	}

	// 결과가 되는 FuzzySet 등록 (오직 하나만 등록 가능)
	void registerResultFuzzySet(FuzzySet fset, string key){
		mFuzzysetResult = fset;
		mFuzzysetResultKey = key;
	}
};

// Rule을 여러개 등록하여 최종적으로 fuzzy inference 연산을 하게 됨
class RuleSet{
private:
	int mRuleArrSize; // Rule이 저장되는 배열의 크기
	int mRuleCount; // 현재 등록된 Rule의 수
	Rule* mRuleArr; // Rule이 저장되는 배열
	map<string, double> mMapMax; // 결과가 되는 FuzzySet 이름과, 그것의 최대값이 매핑됨
	map<string, FuzzySet*> mMapResult; // 결과가 되는 FuzzySet 이름과, 그것의 포인터가 매핑됨
public:
	RuleSet(){
		mRuleArr = NULL;
	}
	RuleSet(int ruleArrSize){
		mRuleArrSize = ruleArrSize;
		mRuleCount = 0;
		mRuleArr = new Rule[ruleArrSize];
	}
	RuleSet(RuleSet& ruleSet){
		copyObject(ruleSet);
	}

	~RuleSet(){
		if (mRuleArr != NULL) delete[] mRuleArr;
	}

	void operator=(RuleSet& ruleSet){
		if (mRuleArr != NULL){
			delete[] mRuleArr;
			mRuleArr = NULL;
		}
		copyObject(ruleSet);
	}

	// 객체를 복사하는 함수. 복사생성자와 =연산에서 사용됨
	void copyObject(RuleSet& ruleSet){
		mRuleArrSize = ruleSet.mRuleArrSize;
		mRuleCount = ruleSet.mRuleCount;
		if (ruleSet.mRuleArr != NULL){
			mRuleArr = new Rule[ruleSet.mRuleArrSize];
			for (int i = 0; i < ruleSet.mRuleArrSize; i++){
				mRuleArr[i] = ruleSet.mRuleArr[i];
			}
		}
		else mRuleArr = NULL;
	}

	// RuleSet에 Rule을 추가함
	void addRule(Rule rule){
		if (mRuleCount == mRuleArrSize)
			printf("error: Rule Array is full\n");
		else{
			mRuleArr[mRuleCount] = rule;
			mRuleCount++;
		}
	}

	void getResultValue(vector<double> cond){
		int condSize;
		double minVal;

		printf("===== getResultValue Result =====\n");

		for (int i = 0; i < mRuleCount; i++){
			minVal = 99999;
			condSize = mRuleArr[i].getCondSize();
			for (int j = 0; j < condSize; j++){
				minVal = min(minVal, mRuleArr[i].getCondFuzzySet(j)->getValue(cond[j]));
				//printf("    %lf ==> %lf\n", cond[j], mRuleArr[i].getCondFuzzySet(j)->getValue(cond[j]));
			}
			printf("  Rule %d: %s = %lf\n", i+1, mRuleArr[i].getFuzzySetResultKey().c_str(), minVal);
			
			mMapResult[mRuleArr[i].getFuzzySetResultKey()] = mRuleArr[i].getFuzzysetResult();

			if (mMapMax[mRuleArr[i].getFuzzySetResultKey()] < minVal)
				mMapMax[mRuleArr[i].getFuzzySetResultKey()] = minVal;
		}
		printf("\n");
		for (map<string, double>::iterator i = mMapMax.begin(); i != mMapMax.end(); i++)
			printf("  max(\"%s\"): %lf\n", i->first.c_str(), i->second);

		printf("\n  defuzzification: %lf\n", defuzzification(15, 300));

		printf("=================================\n");
	}
	
	// defuzzification() 에서 사용되는 함수. y = f(x)
	double resultFunction(double x){
		string fsetResultName;
		double memberFuncMax, maxVal = 0, tempVal;
		FuzzySet* fsetResult;

		for (map<string, double>::iterator i = mMapMax.begin(); i != mMapMax.end(); i++){
			fsetResultName = i->first;
			memberFuncMax = i->second;
			fsetResult = mMapResult[fsetResultName];
			tempVal = (fsetResult->getValue(x) > memberFuncMax) ? memberFuncMax : fsetResult->getValue(x);
			maxVal = max(maxVal, tempVal);
		}

		return maxVal;
	}

	// 역퍼지화 함수
	// 주의 : 반드시 getResultValue() 함수 후에 사용할 것!
	double defuzzification(double endVal, int resolution){
		double top = 0, bottom = 0;
		double x = 0;
		
		for (int i = 0; i < resolution; i++){
			//printf("  defuzz: x = %lf, y = %lf\n", x, resultFunction(x));
			top += resultFunction(x) * x;
			bottom += resultFunction(x);
			x += endVal/(resolution-1);
		}

		return top/bottom;
	}
};

int main(){
	// 1. 먼저 각 FuzzySet에 대한 Membership Function을 만든다.
	// 종형 함수를 사용한다. 파라미터는 (너비, 중심점 좌표, 최대값) 순이다.
	BellMemberFunction mfA1(100, 0, 1), mfA2(70, 45, 1), mfA3(70, 70, 1);
	BellMemberFunction mfB1(7, 36.5, 1), mfB2(7, 39, 1), mfB3(7, 41, 1);
	BellMemberFunction mfC1(18, 0, 1), mfC2(22, 15, 1);
	
	// 2. FuzzySet을 정의하고 Membership Function을 등록한다.
	FuzzySet A1, A2, A3, B1, B2, B3, C1, C2;
	A1.setMemberFunction(&mfA1);
	A2.setMemberFunction(&mfA2);
	A3.setMemberFunction(&mfA3);
	B1.setMemberFunction(&mfB1);
	B2.setMemberFunction(&mfB2);
	B3.setMemberFunction(&mfB3);
	C1.setMemberFunction(&mfC1);
	C2.setMemberFunction(&mfC2);

	// 3. 규칙을 정의한다. (9가지)
	Rule rule[9];
	for (int i = 0; i < 9; i++){
		rule[i] = Rule(2); // Rule(2)인 이유는 각 규칙마다 조건이 2개 들어가기 때문
	}
	//  ex. A1 이고 B2이면 C1이다.
	rule[0].addCondFuzzySet(A1); // 조건 1
	rule[0].addCondFuzzySet(B1); // 조건 2
	rule[0].registerResultFuzzySet(C1, "C1"); // 결과

	rule[1].addCondFuzzySet(A1);
	rule[1].addCondFuzzySet(B2);
	rule[1].registerResultFuzzySet(C1, "C1");

	rule[2].addCondFuzzySet(A1);
	rule[2].addCondFuzzySet(B3);
	rule[2].registerResultFuzzySet(C2, "C2");

	rule[3].addCondFuzzySet(A2);
	rule[3].addCondFuzzySet(B1);
	rule[3].registerResultFuzzySet(C1, "C1");

	rule[4].addCondFuzzySet(A2);
	rule[4].addCondFuzzySet(B2);
	rule[4].registerResultFuzzySet(C1, "C1");

	rule[5].addCondFuzzySet(A2);
	rule[5].addCondFuzzySet(B3);
	rule[5].registerResultFuzzySet(C2, "C2");

	rule[6].addCondFuzzySet(A3);
	rule[6].addCondFuzzySet(B1);
	rule[6].registerResultFuzzySet(C2, "C2");

	rule[7].addCondFuzzySet(A3);
	rule[7].addCondFuzzySet(B2);
	rule[7].registerResultFuzzySet(C2, "C2");

	rule[8].addCondFuzzySet(A3);
	rule[8].addCondFuzzySet(B3);
	rule[8].registerResultFuzzySet(C2, "C2");

	// 4. 앞에서 만든 9개의 규칙을 RuleSet에 집어넣는다.
	RuleSet ruleSet(9);
	for (int i = 0; i < 9; i++){
		ruleSet.addRule(rule[i]);
	}

	vector<double> cond = { 40, 36.5 }; // 강우량 40, 체온 36.5
	ruleSet.getResultValue(cond); // fuzzy inference 연산을 취한다.

	return 0;
}