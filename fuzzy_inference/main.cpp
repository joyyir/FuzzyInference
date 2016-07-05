#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
using namespace std;

// ����� �Լ� inferface
// ��� ����� �Լ��� �� �������̽��� ��ӹ޴´�.
class MembershipFunction{
public:
	virtual double getValue(double x) = 0; // y = f(x)
};

// �ﰢ����� ����� �Լ�
class TriangleMemberFunction : public MembershipFunction{
private:
	double mWidth; // �ʺ�
	double mCenter; // �߽��� ��ǥ
	double mPeak; // �ְ��� �Լ���
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
	double mWidth; // �ʺ�
	double mCenter; // �߽��� ��ǥ
	double mPeak; // �ְ��� �Լ���
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

// FuzzySet�� �����Ѵ�.
// ���� ���, if X is A1 and Y is B2, then W is C1 �̶�� ��Ģ�� �ִٸ�
// A1, B2, C1�� FuzzySet�� �ش�ȴ�.
// �� FuzzySet�� ����� �Լ��� ���´�.
class FuzzySet{
private:
	MembershipFunction *mMemberFunc; // ����� �Լ�
public:
	FuzzySet(){
		mMemberFunc = NULL;
	}

	// ����� �Լ��� ����Ѵ�.
	void setMemberFunction(MembershipFunction* memberFunc){
		mMemberFunc = memberFunc;
	};

	// ����� �Լ��� ����� ��ȯ. y = f(x)
	double getValue(double x){
		if (mMemberFunc != NULL)
			return mMemberFunc->getValue(x);
		else{
			printf("error: Member Function is not assigned\n");
			return 0;
		}
	}
};

// ��Ģ�� �����Ѵ�.
// ���� ���, if X is A1 and Y is B2, then W is C1 �̶�� ������ ��Ģ�̴�.
// �� ��Ģ���� ������ ���� A1, B2 �� 2���̹Ƿ� mCondSize == 2 �ȴ�.
class Rule{
private:
	int mCondSize; // �����̵Ǵ� FuzzySet���� ��� �迭�� ũ��
	int mFuzzysetArrCount; // ���� ��ϵ� ���� FuzzySet�� ��. �ִ�� mCondSize���� Ŀ�� �� ����
	FuzzySet* mFuzzysetArr; // �����̵Ǵ� FuzzySet���� ��� �迭
	FuzzySet mFuzzysetResult; // C1�̶�� FuzzySet�� ����� ��Ÿ���ٸ� C1�� ������
	string mFuzzysetResultKey; // C1�̶�� FuzzySet�� ����� ��Ÿ���ٸ� ���ڿ� "C1"�� ������
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

	// ��ü�� �����ϴ� �Լ�. ��������ڿ� =���꿡�� ����
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

	// ������ �Ǵ� FuzzySet ���
	void addCondFuzzySet(FuzzySet fset){
		if (mFuzzysetArrCount == mCondSize)
			printf("error: FuzzySet Array is full\n");
		else{
			mFuzzysetArr[mFuzzysetArrCount] = fset;
			mFuzzysetArrCount++;
		}
	}

	// ������ �Ǵ� FuzzySet �迭����, Ư�� index�� FuzzySet�� ��ȯ��
	FuzzySet* getCondFuzzySet(int index){
		if (mFuzzysetArr != NULL && mFuzzysetArrCount > index)
			return &mFuzzysetArr[index];
		else{
			printf("error: getCondFuzzySet() failed!\n");
			return NULL;
		}
	}

	// mCondSize ��ȯ
	int getCondSize(){
		return mCondSize;
	}

	// mFuzzysetResultKey ��ȯ
	string getFuzzySetResultKey(){
		return mFuzzysetResultKey;
	}
	
	// mFuzzysetResult ��ȯ
	FuzzySet* getFuzzysetResult(){
		return &mFuzzysetResult;
	}

	// ����� �Ǵ� FuzzySet ��� (���� �ϳ��� ��� ����)
	void registerResultFuzzySet(FuzzySet fset, string key){
		mFuzzysetResult = fset;
		mFuzzysetResultKey = key;
	}
};

// Rule�� ������ ����Ͽ� ���������� fuzzy inference ������ �ϰ� ��
class RuleSet{
private:
	int mRuleArrSize; // Rule�� ����Ǵ� �迭�� ũ��
	int mRuleCount; // ���� ��ϵ� Rule�� ��
	Rule* mRuleArr; // Rule�� ����Ǵ� �迭
	map<string, double> mMapMax; // ����� �Ǵ� FuzzySet �̸���, �װ��� �ִ밪�� ���ε�
	map<string, FuzzySet*> mMapResult; // ����� �Ǵ� FuzzySet �̸���, �װ��� �����Ͱ� ���ε�
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

	// ��ü�� �����ϴ� �Լ�. ��������ڿ� =���꿡�� ����
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

	// RuleSet�� Rule�� �߰���
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
	
	// defuzzification() ���� ���Ǵ� �Լ�. y = f(x)
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

	// ������ȭ �Լ�
	// ���� : �ݵ�� getResultValue() �Լ� �Ŀ� ����� ��!
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
	// 1. ���� �� FuzzySet�� ���� Membership Function�� �����.
	// ���� �Լ��� ����Ѵ�. �Ķ���ʹ� (�ʺ�, �߽��� ��ǥ, �ִ밪) ���̴�.
	BellMemberFunction mfA1(100, 0, 1), mfA2(70, 45, 1), mfA3(70, 70, 1);
	BellMemberFunction mfB1(7, 36.5, 1), mfB2(7, 39, 1), mfB3(7, 41, 1);
	BellMemberFunction mfC1(18, 0, 1), mfC2(22, 15, 1);
	
	// 2. FuzzySet�� �����ϰ� Membership Function�� ����Ѵ�.
	FuzzySet A1, A2, A3, B1, B2, B3, C1, C2;
	A1.setMemberFunction(&mfA1);
	A2.setMemberFunction(&mfA2);
	A3.setMemberFunction(&mfA3);
	B1.setMemberFunction(&mfB1);
	B2.setMemberFunction(&mfB2);
	B3.setMemberFunction(&mfB3);
	C1.setMemberFunction(&mfC1);
	C2.setMemberFunction(&mfC2);

	// 3. ��Ģ�� �����Ѵ�. (9����)
	Rule rule[9];
	for (int i = 0; i < 9; i++){
		rule[i] = Rule(2); // Rule(2)�� ������ �� ��Ģ���� ������ 2�� ���� ����
	}
	//  ex. A1 �̰� B2�̸� C1�̴�.
	rule[0].addCondFuzzySet(A1); // ���� 1
	rule[0].addCondFuzzySet(B1); // ���� 2
	rule[0].registerResultFuzzySet(C1, "C1"); // ���

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

	// 4. �տ��� ���� 9���� ��Ģ�� RuleSet�� ����ִ´�.
	RuleSet ruleSet(9);
	for (int i = 0; i < 9; i++){
		ruleSet.addRule(rule[i]);
	}

	vector<double> cond = { 40, 36.5 }; // ���췮 40, ü�� 36.5
	ruleSet.getResultValue(cond); // fuzzy inference ������ ���Ѵ�.

	return 0;
}