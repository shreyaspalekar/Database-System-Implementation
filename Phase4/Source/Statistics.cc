#include "Statistics.h"
#include <iostream>
#include <map>
#include <set>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string.h>
#include <iomanip>
#include <typeinfo> 
#define _DEP
//#define _DEBUG


Statistics::Statistics()
{

	isCalledFrmApply = false;
	isApply = false;
	relationData = new map<string,int>();
	attrData= new map<string,map <string, int> >();

}


Statistics::Statistics(Statistics &copyMe)
{

	relationData = new map<string,int>(*(copyMe.relationData));
	attrData = new map<string,map <string, int> >(*(copyMe.attrData));

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

	pair < map<string,int>::iterator, bool> ret = relationData->insert(pair<string,int>(rel,numTuples));

	if(ret.second==false){

		cout<<"Duplicate found";

		relationData->erase(ret.first);
		relationData->insert(pair<string,int>(rel,numTuples));

	}

}
void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
	
	string aName(attName);
	string rname(relName);


	if (numDistincts == -1) {

                int numTuples = relationData->at(rname);
                (*attrData)[rname][aName] = numTuples;

        } else {
                (*attrData)[rname][aName] = numDistincts;
        }	
	

}
void Statistics::CopyRel(char *_oldName, char *_newName)
{


	string oldName(_oldName);
        string newName(_newName);

	//copy relation data

        int oldNumTuples = (*relationData)[oldName];
        (*relationData)[newName] = oldNumTuples;

	//copy relation attribute 

	map<string, int> &oldattrData = (*attrData)[oldName];

        for (map<string, int>::iterator oldAttrInfo = oldattrData.begin(); oldAttrInfo != oldattrData.end(); ++oldAttrInfo) {
                
		string newAtt = newName;
                newAtt += "." + oldAttrInfo->first;
                //cout << (*oldAttrInfo).first << ": " << (*oldAttrInfo).second << endl;
                (*attrData)[newName][newAtt] = oldAttrInfo->second;
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
        relationData->clear();

	//2.read actual relationdata map
        for (int i = 0; i < relationDataSize; i++) {

                readFile >> input;

                size_t splitAt = input.find_first_of("#");
                string part1 = input.substr(0, splitAt);
                string part2 = input.substr(splitAt + 1);


                int part2Int = atoi(part2.c_str());
                (*relationData)[part1] = part2Int;

        }

	//3.skip over attrData map size
        readFile >> input;

	//clear attrData map
        attrData->clear();

	//4. read in actual attrData map
        string relName, attrName, distinctCount;
        readFile >> relName >> attrName >> distinctCount;

        while (!readFile.eof()) {

                int distinctCountInt = atoi(distinctCount.c_str());
                (*attrData)[relName][attrName] = distinctCountInt;
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

        int relationDataSize = relationData->size();
        writeFile << relationDataSize << "\n";

	//2. actual relation data map

        for (map<string, int>::iterator entry = relationData->begin(); entry != relationData->end(); entry++) {

                const char *first = entry->first.c_str();
                int second = entry->second;

                writeFile << first << "#" << second << "\n";


        }


	//3. attrData size

        int attrDataSize = attrData->size();
        writeFile << attrDataSize << "\n";

	//4. actual attrData map

        for (map<string, map<string, int> >::iterator ii = attrData->begin(); ii != attrData->end(); ++ii) {

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

		isCalledFrmApply = true;
        isApply = true;
        Estimate(parseTree, relNames, numToJoin);
        isApply = false;
        isCalledFrmApply = false;

}

double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{


    double resultEstimate = 0.0;
    // TODO error checking
    struct AndList *currentAnd;
    struct OrList *currentOr;

    currentAnd = parseTree;

    string leftRelation;
    string rightRelation;

    string leftAttr;
    string rightAttr;

    string joinLeftRelation, joinRightRelation;

    bool isJoin = false;
    bool isJoinPerformed = false;

    bool isdep = false;
    bool done = false;
    string prev;

    double resultANDFactor = 1.0;
    double resultORFactor = 1.0;

    map<string, int> relOpMap;

//And list is structured as a root, a orlist the left and andlist to the right.
//Or list is structured as a root, a comparison the left and orlist to the right.
//a comparison is structed as a root and operands to the left and right.
//operands consists of a code and value.

    while (currentAnd != NULL) {


            currentOr = currentAnd->left;
            resultORFactor = 1.0;


           while (currentOr != NULL) {


                    isJoin = false;
                    ComparisonOp *currentCompOp = currentOr->left;


                    // find relation of left attribute
		//first attribute has to be a name
                    if (currentCompOp->left->code != NAME) {
                            cout << "LEFT should be attribute name" << endl;

                            return 0;
                    } else {

			//find the relation where the attribute lies.
				
                            leftAttr = currentCompOp->left->value;

				#ifdef _DEP
				if(strcmp(leftAttr.c_str(),prev.c_str())==0)
				{

					//cout<<"equal"<<endl;
					isdep=true;
				}

				prev= leftAttr;
				#endif


                            //cout << "Left Attribute is " << leftAttr << endl;



                            for (map<string, map<string, int> >::iterator mapEntry = attrData->begin(); mapEntry != attrData->end(); mapEntry++) {
                                    if ((*attrData)[mapEntry->first].count(leftAttr) > 0) {

                                            leftRelation = mapEntry->first;
                                            break;
                                    }

                            }

                    }

                    // find relation of right attribute
                    if (currentCompOp->right->code == NAME) {//the right operand is a name too hence it is a join
                            isJoin = true;
                            isJoinPerformed = true;
                            rightAttr = currentCompOp->right->value;

			//find right relation

                            for (map<string, map<string, int> >::iterator mapEntry = attrData->begin(); mapEntry != attrData->end(); ++mapEntry) {
                                    if ((*attrData)[mapEntry->first].count(rightAttr) > 0) {
                                            rightRelation = mapEntry->first;
                                            break;
                                    }
                            }
                    }

                    if (isJoin == true) {

			//find distinct counts of both attributes for the relations.

                            double leftDistinctCount = (*attrData)[leftRelation][currentCompOp->left->value];
                            double rightDistinctCount = (*attrData)[rightRelation][currentCompOp->right->value];

                            if (currentCompOp->code == EQUALS) {
                                    resultORFactor *=(1.0 - (1.0 / max(leftDistinctCount, rightDistinctCount)));//ORFACTOR??
                            }

                            joinLeftRelation = leftRelation;
                            joinRightRelation = rightRelation;

                    } else {
		
			#ifdef _DEP
			if(isdep){


			

				if(!done){
					
					//cout<<"done"<<endl;
					resultORFactor =1.0 -resultORFactor;
					done = true;


				}

			    if (currentCompOp->code == GREATER_THAN || currentCompOp->code == LESS_THAN) {
                                    resultORFactor += (1.0 / 3.0);
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;

                            }
                            if (currentCompOp->code == EQUALS) {
                                    resultORFactor +=(1.0 / ((*attrData)[leftRelation][currentCompOp->left->value]));
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;
                            }


			#ifdef _DEBUG
			            cout<<"or "<<std::setprecision (15) <<resultORFactor<<endl;
				    cout<<"ikr "<< (*attrData)[leftRelation][currentCompOp->left->value]<<endl;

		            #endif

				
				


			}
			




                        else{

                            if (currentCompOp->code == GREATER_THAN || currentCompOp->code == LESS_THAN) {
                                    resultORFactor *= (2.0 / 3.0);
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;

                            }
                            if (currentCompOp->code == EQUALS) {
                                    resultORFactor *=(1.0- (1.0 / (*attrData)[leftRelation][currentCompOp->left->value]));
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;
                            }

				#ifdef _DEBUG
			            cout<<"or"<<resultORFactor<<endl;
					cout<<"ikr "<< (*attrData)[leftRelation][currentCompOp->left->value]<<endl;


		            #endif



			}

			#else


			if (currentCompOp->code == GREATER_THAN || currentCompOp->code == LESS_THAN) {
                                    resultORFactor *= (2.0 / 3.0);
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;

                            }
                            if (currentCompOp->code == EQUALS) {
                                    resultORFactor *=(1.0- (1.0 / (*attrData)[leftRelation][currentCompOp->left->value]));
                                    relOpMap[currentCompOp->left->value] = currentCompOp->code;
                            }

			#endif


				



                    }
                    currentOr = currentOr->rightOr;
            }
	
      


	    #ifdef _DEP
	    if(!isdep)
		resultORFactor =1.0 -resultORFactor;	

            #else
	    resultORFactor =1.0 -resultORFactor;
	    #endif

		#ifdef _DEBUG
			cout<<"prev and"<<resultANDFactor <<" or "<< resultORFactor<<"curr and "<< resultANDFactor*resultORFactor<<endl;

		#endif

		      isdep=false;
	    done =false;

            resultANDFactor *= resultORFactor;
            currentAnd = currentAnd->rightAnd;
    }


    double rightTupleCount = (*relationData)[rightRelation];

    if (isJoinPerformed == true) {
            double leftTupleCount = (*relationData)[joinLeftRelation];
            resultEstimate = leftTupleCount * rightTupleCount * resultANDFactor;
    } else {
            double leftTupleCount = (*relationData)[leftRelation];
            resultEstimate = leftTupleCount * resultANDFactor;
    }

    if (isApply) {

//    		cout<<"is apply is" <<isApply;

            map<string, int>::iterator relOpMapITR, distinctCountMapITR;
            set<string> addedJoinAttrSet;
            if (isJoinPerformed) {
                    for (relOpMapITR = relOpMap.begin(); relOpMapITR != relOpMap.end(); relOpMapITR++) {

                            for (int i = 0; i < relationData->size(); i++) {
                                    if (relNames[i] == NULL)
                                            continue;
                                    int cnt = ((*attrData)[relNames[i]]).count(relOpMapITR->first);
                                    if (cnt == 0)
                                            continue;
                                    else if (cnt == 1) {

                                            for (distinctCountMapITR = (*attrData)[relNames[i]].begin(); distinctCountMapITR != (*attrData)[relNames[i]].end(); distinctCountMapITR++) {
                                                    if ((relOpMapITR->second == LESS_THAN) || (relOpMapITR->second == GREATER_THAN)) {
                                                    	(*attrData)[joinLeftRelation + "_" + joinRightRelation][distinctCountMapITR->first] = (int)round((double)(distinctCountMapITR->second) / 3.0);
                                                    } else if (relOpMapITR->second == EQUALS) {
                                                            if (relOpMapITR->first == distinctCountMapITR->first) { //same attribute on which condition is imposed
                                                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][distinctCountMapITR->first] = 1;
                                                            } else
                                                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][distinctCountMapITR->first] = min((int)round(resultEstimate), distinctCountMapITR->second);
                                                    }
                                            }
                                            break;
                                    } else if (cnt > 1) {

                                            for (distinctCountMapITR = (*attrData)[relNames[i]].begin(); distinctCountMapITR != (*attrData)[relNames[i]].end(); distinctCountMapITR++) {
                                                    if (relOpMapITR->second == EQUALS) {
                                                            if (relOpMapITR->first == distinctCountMapITR->first) {
                                                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][distinctCountMapITR->first] = cnt;
                                                            } else
                                                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][distinctCountMapITR->first] = min((int) round(resultEstimate), distinctCountMapITR->second);
                                                    }
                                            }
                                            break;
                                    }
                                    addedJoinAttrSet.insert(relNames[i]);
                            }
                    }

                    if (addedJoinAttrSet.count(joinLeftRelation) == 0) {
                            for (map<string, int>::iterator entry = (*attrData)[joinLeftRelation].begin(); entry != (*attrData)[joinLeftRelation].end(); entry++) {
                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][entry->first] = entry->second;

                            }
                    }
                    if (addedJoinAttrSet.count(joinRightRelation) == 0) {
                            for (map<string, int>::iterator entry = (*attrData)[joinRightRelation].begin(); entry != (*attrData)[joinRightRelation].end(); entry++) {
                            	(*attrData)[joinLeftRelation + "_" + joinRightRelation][entry->first] = entry->second;

                            }
                    }
                    (*relationData)[joinLeftRelation + "_" + joinRightRelation] =round(resultEstimate);
                    relationData->erase(joinLeftRelation);
                    relationData->erase(joinRightRelation);

                    attrData->erase(joinLeftRelation);
                    attrData->erase(joinRightRelation);

            }
    }
    return resultEstimate;


}

