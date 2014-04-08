#include "Statistics.h"

Statistics::Statistics()
{
}
Statistics::Statistics(Statistics &copyMe)
{
}
Statistics::~Statistics()
{
}

void Statistics::AddRel(char *relName, int numTuples)
{

//	if(relationData.find(relName)){

		relData n = new relData;
		n.numTuples = numTuples;

		relationData.insert(pair<char*,relData>(relName,n));


//	}

}
void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
}
void Statistics::CopyRel(char *oldName, char *newName)
{
}
	
void Statistics::Read(char *fromWhere)
{
}
void Statistics::Write(char *fromWhere)
{
}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{
}
double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{
}

