#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
using namespace std;

class MembershipFunction{
public:
	virtual double getValue(double x) = 0;
};

class TriangleMemberFunction : public MembershipFunction{
private:
	double mWidth;
	double mCenter;
	double mPeak;
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
	double mWidth;
	double mCenter;
	double mPeak;
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

class FuzzySet{
private:
	MembershipFunction *mMemberFunc;
public:
	FuzzySet(){
		mMemberFunc = NULL;
	}

	void setMemberFunction(MembershipFunction* memberFunc){
		mMemberFunc = memberFunc;
	};

	double getValue(double x){
		if (mMemberFunc != NULL)
			return mMemberFunc->getValue(x);
		else{
			printf("error: Member Function is not assigned\n");
			return 0;
		}
	}
};

class Rule{
private:
	int mCondSize;
	int mFuzzysetArrCount;
	FuzzySet* mFuzzysetArr;
	FuzzySet mFuzzysetResult;
	string mFuzzysetResultKey;
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

	void addCondFuzzySet(FuzzySet fset){
		if (mFuzzysetArrCount == mCondSize)
			printf("error: FuzzySet Array is full\n");
		else{
			mFuzzysetArr[mFuzzysetArrCount] = fset;
			mFuzzysetArrCount++;
		}
	}

	FuzzySet* getCondFuzzySet(int index){
		if (mFuzzysetArr != NULL && mFuzzysetArrCount > index)
			return &mFuzzysetArr[index];
		else{
			printf("error: getCondFuzzySet() failed!\n");
			return NULL;
		}
	}

	int getCondSize(){
		return mCondSize;
	}

	string getFuzzySetResultKey(){
		return mFuzzysetResultKey;
	}

	FuzzySet* getFuzzysetResult(){
		return &mFuzzysetResult;
	}

	void registerResultFuzzySet(FuzzySet fset, string key){
		mFuzzysetResult = fset;
		mFuzzysetResultKey = key;
	}
};

class RuleSet{
private:
	int mRuleArrSize;
	int mRuleCount;
	Rule* mRuleArr;
	map<string, double> mMapMax;
	map<string, FuzzySet*> mMapResult;
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

	double defuzzification(double endVal, int resolution){
		double top = 0, bottom = 0;
		double x = 0, y;
		
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
	BellMemberFunction mfA1(100, 0, 1), mfA2(70, 45, 1), mfA3(70, 70, 1);
	BellMemberFunction mfB1(7, 36.5, 1), mfB2(7, 39, 1), mfB3(7, 41, 1);
	BellMemberFunction mfC1(18, 0, 1), mfC2(22, 15, 1);
	
	FuzzySet A1, A2, A3, B1, B2, B3, C1, C2;
	A1.setMemberFunction(&mfA1);
	A2.setMemberFunction(&mfA2);
	A3.setMemberFunction(&mfA3);
	B1.setMemberFunction(&mfB1);
	B2.setMemberFunction(&mfB2);
	B3.setMemberFunction(&mfB3);
	C1.setMemberFunction(&mfC1);
	C2.setMemberFunction(&mfC2);

	Rule rule[9];
	for (int i = 0; i < 9; i++){
		rule[i] = Rule(2);
	}
	rule[0].addCondFuzzySet(A1);
	rule[0].addCondFuzzySet(B1);
	rule[0].registerResultFuzzySet(C1, "C1");

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

	RuleSet ruleSet(9);
	for (int i = 0; i < 9; i++){
		ruleSet.addRule(rule[i]);
	}

	vector<double> cond = { 40, 36.5 };
	ruleSet.getResultValue(cond);

	return 0;
}