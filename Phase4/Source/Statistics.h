#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <string>
#include <map>

/*struct cmp_str
{
   bool operator()(char const *a, char const *b)
   {
      return std::strcmp(a, b) < 0;
   }
};*/

/*typedef struct relData{

	int numTuples;
	


}reldata;*/

using namespace std;

class Statistics
{

	map<string,int> *relationData;
	map<string,map <string, int> > *attrData;
	bool isCalledFrmApply;
	bool isApply;

public:
	Statistics();
	Statistics(Statistics &copyMe);	 // Performs deep copy
	~Statistics();


	void AddRel(char *relName, int numTuples);
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);
	
	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);

};

#endif
