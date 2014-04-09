#include "Statistics.h"

Statistics::Statistics()
{

	relationData = new map<string,int>();
	attrData= new map<string,map <string, int>>();

}


Statistics::Statistics(Statistics &copyMe)
{

	relationData = new map<string,int>(copyMe.relationData);
	attrData = new map<string,map <string, int>>(copyMe.attrData);

}


Statistics::~Statistics()
{
	delete relationData;
	delete attrData;
}

void Statistics::AddRel(char *relName, int numTuples)
{

	//relData n = new relData;
	//n.numTuples = numTuples;



	string rel(relName);

	pair < map<string,int>::iterator, bool> ret = relationData.insert(pair<string,int>(rel,numTuples));

	if(ret.second==false){

		cout<<"Duplicate found";

		relationData.erase(ret.first);
		relationData.insert(pair<string,int>(rel,numTuples));

	}

}
void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
	
	string aName(attName);
	string rname(relName);


	if (numDistincts == -1) {

                int numTuples = relationData.at(rel);
                attrData[rel][att] = numTuples;

        } else {
                attrData[rel][att] = numDistincts;
        }	
	

}
void Statistics::CopyRel(char *oldName, char *newName)
{


	string oldName(_oldName);
        string newName(_newName);

	//copy relation data

        int oldNumTuples = relationData[oldName];
        relationData[newName] = oldNumTuples;

	//copy relation attribute 

	map<string, int> &oldattrData = attrData[oldName];

        for (map<string, int>::iterator oldAttrInfo = oldattrData.begin(); oldAttrInfo != oldattrData.end(); ++oldAttrInfo) {
                
		string newAtt = newName;
                newAtt += "." + oldattrData->first;
                //cout << (*oldAttrInfo).first << ": " << (*oldAttrInfo).second << endl;
                attrData[newName][newAtt] = oldattrData->second;
        }


}
	
void Statistics::Read(char *fromWhere)
{

	string fileName(fromWhere);

        ifstream ifile(fromWhere);


        if (!ifile) {//TODO::create blank file and return
                cout << "FILE doest not exist";
                return;
        }

        ifstream readFile;

        readFile.open(fileName.c_str(), ios::in);

	//1.read relationData size 
        string input;
        readFile >> input;
        int relationDataSize = atoi(input.c_str());

	//clear relation data
        relationData.clear();

	//2.read actual relationdata map
        for (int i = 0; i < relationDataSize; i++) {

                readFile >> input;

                size_t splitAt = input.find_first_of("#");
                string part1 = input.substr(0, splitAt);
                string part2 = input.substr(splitAt + 1);


                int part2Int = atoi(part2.c_str());
                relationData[part1] = part2Int;

        }

	//3.skip over attrData map size
        readFile >> input;

	//clear attrData map
        attrData.clear();

	//4. read in actual attrData map
        string relName, attrName, distinctCount;
        readFile >> relName >> attrName >> distinctCount;

        while (!readFile.eof()) {

                int distinctCountInt = atoi(distinctCount.c_str());
                attrData[relName][attrName] = distinctCountInt;
                readFile >> relName >> attrName >> distinctCount;

        }


        readFile.close();



}

void Statistics::Write(char *fromWhere)
{

        string fileName(fromWhere);
        remove(fromWhere);

        ofstream writeFile;
        writeFile.open(fileName.c_str(), ios::out);


	//1.relation data size

        int relationDataSize = relationData.size();
        writeFile << relationDataSize << "\n";

	//2. actual relation data map

        for (map<string, int>::iterator entry = relationData.begin(); entry != relationData.end(); entry++) {

                const char *first = entry->first.c_str();
                int second = entry->second;

                writeFile << first << "#" << second << "\n";


        }


	//3. attrData size

        int attrDataSize = attrData.size();
        writeFile << attrDataSize << "\n";

	//4. actual attrData map

        for (map<string, map<string, int> >::iterator ii = attrData.begin(); ii != attrData.end(); ++ii) {

                for (map<string, int>::iterator j = ii->second.begin(); j != ii->second.end(); ++j) {
                        //cout << (*ii).first << " : " << (*j).first << " : " << (*j).second << endl;
			//1. rel name 2.attr name 3.distincts
                        const char *first = (*ii).first.c_str();
                        const char *second = (*j).first.c_str();
                        int third = (*j).second;
                        writeFile << first << " " << second << " " << third << "\n";

                }

        }

        writeFile.close();




}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{



}

double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{


}

