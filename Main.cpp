/*	Tristan Pior
	CS4375.002
	Assignment 2 Part 2
	Naive Bayes classifier in C++
*/

//Imports
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>

//Constants
#define NUM_CLASS_VALUES 2

using namespace std;

//Object contains a vector of its attribute values in the same order as attributeNames, and its class value
//Used code from my A1Part2 submission
class Item {
private: vector<int> attributeValues;	//Holds the values of each attribute for a single item (values binary: 0 or 1)
private: int classVal;	//Holds the class value for the item
		 //Empty constructor
public: Item() {
	attributeValues = {};	//Instantiate an empty vector
	classVal = -1;	//Instantiate an impossible class value
}
		//Parameter Constructor
public: Item(vector<int> attVals, int classV) {
	classVal = classV;
	attributeValues = attVals;
}

		//Set function for class val if class val is not set already
public: void setClassVal(int val) {
	if (classVal == -1) { classVal = val; }
}
		//Add function to add an attribute value to the Item
public: void addAttributeVal(int val) { attributeValues.push_back(val); }
		//Get function to get the class value
public: int getClassVal() { return classVal; }
		//Get function to get the attribute values
public: vector<int> getAttValues() { return attributeValues; }

};

class AttributeProbability {
	//All attribute values are binary (0 or 1), thus I will have two variables, one for each
private: double probability0;	//Holds the probability of the attribute with a value of 0
private: double probability1;	//Holds the probability of the attribute with a value of 1

		 //Constructor inputs the two probabilities and instantiates
public: AttributeProbability(double val0, double val1) {
	probability0 = val0;
	probability1 = val1;
}

		//Get functions for probabilities
public: double getProbability0() { return probability0; }
public: double getProbability1() { return probability1; }
};

class NaiveBayes {
	private: vector<Item> itemList;	//Holds all Item objects
	private: vector<Item*> nb[NUM_CLASS_VALUES];	//Holds pointers to the Item objects in each class (index 0 for class 0, index 1 for class 1 ...)
	private: vector<AttributeProbability> ap[NUM_CLASS_VALUES];	//Holds the AttributeProbability objects in the same order as attributeNames for each attribute for each class (index as above)
	private: double cp[NUM_CLASS_VALUES];	//Holds the overall probability of each class (index as above)
	private: vector<string> attributeNames; //Holds the strings containing each of the attribute names
	private: int mostFrequentClass;

	public: NaiveBayes() {
		itemList = {};	//Instantiate empty vector
		attributeNames = {};	//Instantiate empty vector
		mostFrequentClass = -1;	//Instantiate with impossible class value
		//Instantiate empty vectors/class probabilities
		for (int i = 0; i < NUM_CLASS_VALUES; i++) {
			nb[i] = {};
			ap[i] = {};
			cp[i] = -1;
		}
	}

		//Clears the data in nb and itemList
	public: void clearData() {
		itemList.clear();	//Empty itemList if it wasn't already
		//Empty nb if it wasn't already
		for (int i = 0; i < NUM_CLASS_VALUES; i++) { nb[i].clear(); }
	}
		//Inputs data from the file fileName, used code from my A1Part2 submission
	public: void input(string fileName) {
		clearData();	//Make sure the data fields are empty
		ifstream read(fileName);	//Input stream to read input from file
		string line;	//Holds the input line from read
		string token;	//Holds the token from line
		int count = 0;	//Holds the number of attributes before the class value

		//Counters for the number of each of the two possible class values
		int c0 = 0;
		int c1 = 0;

		getline(read, line);	//Populate line for use in instantiating the string stream ss
		stringstream ss;	//Used for parsing lines
		ss << line;

		//Populate attributes vector and count them
		while (!ss.eof()) {
			ss >> token;
			if (token != "class") {
				attributeNames.push_back(token);	//Add the attribute name to the vector
				token = "";	//Reset token to empty
				count++;	//Increment attribute counter
			}
		}

		//Populate itemList
		while (getline(read, line)) {
			vector<int> attV = {};	//Used to hold the values of each attribute in order of their appearance from left to right
			ss.str("");	//Reset the string stream to accept a new string
			ss.clear();
			ss << line;	//Input the next line
			int temp = 0;
			int i = 0;
			while (i < count) {
				ss >> temp;
				attV.push_back(temp);
				i++;
			}
			int classV = -1;
			ss >> classV;
			//Increment counters for determining most common class value
			switch (classV) {
			case 0: c0++;
				break;
			case 1: c1++;
				break;
			}
			Item it(attV, classV);
			itemList.push_back(it);
		}


	}

		//Trains on the data that was input, then empties nb and itemList in preparation for inputting test data
	public: void train() {
		//Iterate through all items and put them into their correct classes
		for (size_t i = 0; i < itemList.size(); i++) {
			if (itemList[i].getClassVal() == 0) { nb[0].push_back(&itemList[i]); }
			else { nb[1].push_back(&itemList[i]); }
		}
		//Calculate probabilities
		int totalNumItems = itemList.size();
		int c0 = nb[0].size();	//Number of items in class 0
		int c1 = nb[1].size();	//Number of items in class 1
		if (c0 >= c1) { mostFrequentClass = 0; }
		else { mostFrequentClass = 1; }
		cp[0] = (double) c0 / totalNumItems;	//Set the probability of class 0
		cp[1] = (double) c1 / totalNumItems;	//Set the probability of class 1
		//Repeat for both classes
		for (int classNum = 0; classNum < NUM_CLASS_VALUES; classNum++) {
			//Iterate through all items in this class
			vector<int> numEach0Attribute = {};	//Holds the count for values of 0 for each attribute
			//Give numEach0Attribute a 0 for each attribute
			for (size_t i = 0; i < attributeNames.size(); i++) {
				numEach0Attribute.push_back(0);
			}
			int numItems = nb[classNum].size();	//Holds the number of items in the vector
			for (int i = 0; i < numItems; i++) {
				vector<int> itemAtt = nb[classNum].at(i)->getAttValues();
				//Iterate over each attribute in this item
				for (size_t att = 0; att < itemAtt.size(); att++) {
					if (itemAtt.at(att) == 0) { numEach0Attribute.at(att) = numEach0Attribute.at(att) + 1; }
				}
			}
			//Calculate probabilities of each attribute in this class
			for (size_t i = 0; i < numEach0Attribute.size(); i++) {
				double p0 = (double) numEach0Attribute.at(i) / numItems;	//Probability that attribute at i is 0
				double p1 = 1.0 - p0;	//Probability that attribute at i is 1
				AttributeProbability p(p0, p1);	//Create an AttributeProbability object with the probabilities
				ap[classNum].push_back(p);	//Add the probability object to the list
			}
		}
		clearData();
	}

		//Test on input data
	public: void test() {
		double p0;
		double p1;
		//Iterate through all items and add them to their classified class
		for (size_t i = 0; i < itemList.size(); i++) {
			p0 = cp[0];	//Start p0 with the probability of class 0
			p1 = cp[1];	//Start p1 with the probability of class 1
			vector<int> itAtt = itemList.at(i).getAttValues();	//Get attribute list of item i
			for (size_t j = 0; j < itAtt.size(); j++) {
				if (itAtt.at(j) == 0) {
					//Attribute value is 0, calculate accordingly
					p0 = (double)(p0 * ap[0].at(j).getProbability0());	//Probability of being class 0
					p1 = (double)(p1 * ap[1].at(j).getProbability0());	//Probability of being class 1
				}
				else {
					//Attribute value is 1, calculate accordingly
					p0 = (double)(p0 * ap[0].at(j).getProbability1());	//Probability of being class 0
					p1 = (double)(p1 * ap[1].at(j).getProbability1());	//Probability of being class 1
				}
			}
			//Calculated on all attributes, check which class is most likely and add item i to it
			if (p0 > p1) { nb[0].push_back(&itemList.at(i)); }			//More probable to be in class 0, add to class 0
			else if (p1 > p0) { nb[1].push_back(&itemList.at(i)); }		//More probable to be in class 1, add to class 1
			else { nb[mostFrequentClass].push_back(&itemList.at(i)); }	//Classes are tied, add to most frequent class in the entire training set
		}
	}

		//Get accuracy
	public: double getAccuracy() {
		int numCorrect = 0;	//Counter for the total number of correctly classified items
		//Iterate through both classes
		for (int i = 0; i < NUM_CLASS_VALUES; i++) {
			//Iterate though all items in each class
			for (size_t j = 0; j < nb[i].size(); j++) {
				int val = nb[i].at(j)->getClassVal();	//Get class value of item
				if (val == i) { numCorrect++; }	//Check if class value of item matches the class it is in, if so increment numCorrect
			}
		}
		int numTotal = itemList.size();
		return (double) numCorrect / numTotal;
	}

		//Get function to get the total number of items
	public: int getTotalItems() { return itemList.size(); }

		//Print function
	public: void printNaiveBayes() {
		cout << fixed;	//Set cout to fixed floating point notation
		cout << setprecision(2);
		//Class value 0
		cout << "P(class=0)=" << cp[0] << " ";
		//Iterate through all attributes
		for (size_t i = 0; i < attributeNames.size(); i++) {
			cout << "P(" << attributeNames.at(i) << "=0|0)=" << ap[0].at(i).getProbability0() << " ";	//Print probability for attribute value of 0 in class 0
			cout << "P(" << attributeNames.at(i) << "=1|0)=" << ap[0].at(i).getProbability1() << " ";	//Print probability for attribute value of 1 in class 0
		}
		//Class value 1
		cout << "\nP(class=1)=" << cp[1] << " ";
		//Iterate through all attributes
		for (size_t i = 0; i < attributeNames.size(); i++) {
			cout << "P(" << attributeNames.at(i) << "=0|1)=" << ap[1].at(i).getProbability0() << " ";	//Print probability for attribute value of 0 in class 1
			cout << "P(" << attributeNames.at(i) << "=1|1)=" << ap[1].at(i).getProbability1() << " ";	//Print probability for attribute value of 1 in class 1
		}
	}
};

//Main
int main(int argc, char** argv) {
	//If there is an improper number of arguments, then terminate.
	if (argc != 3) {
		cout << "Invalid number of arguments. Requires 1 training file and 1 testing file.";
		return 1;
	}

	//Variables to store the file names
	string trainFile = argv[1];
	string testFile = argv[2];

	NaiveBayes nb;	//Create a NaiveBayes
	nb.input(trainFile);	//Input the training data
	nb.train();	//Train the naive bayes classifier
	nb.printNaiveBayes();
	nb.input(trainFile);	//Reinput the training data
	nb.test();	//Test on the training data
	printf("\nAccuracy on training set (%i instances): %.2f%%\n", nb.getTotalItems(), nb.getAccuracy() * 100);
	nb.clearData();	//Clear the data to input test file data
	nb.input(testFile);	//Input test data
	nb.test();	//Test on the test data
	printf("\nAccuracy on test set (%i instances): %.2f%%\n", nb.getTotalItems(), nb.getAccuracy() * 100);


	//Wait at the end so the user can view the console window
	cout << "Press any key to continue . . .";
	cin.get();
}