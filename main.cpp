#include <bits/stdc++.h>

using namespace std;

class RegularExpressionTokens{
	public:
		virtual string getType()=0;
};

class RegularDefinition : public RegularExpressionTokens{

	private:
		unordered_set<string>data;
		bitset<128> chars;
		string definitionName;
		bool epsilon = false;

	public:

		void setDefinitionName(string name){
			definitionName = name;
		}

        string getDefinitionName (){
            return definitionName;
        }
		void addStrings(string str){
			if(str.length() == 1){
				chars.set(str[0]);
			}else{
				data.insert(str);
			}
		}

		bool isExist(string str){
			if(str.length() == 1){
				return chars.test(str[0]);
			}else{
				return data.count(str) == 1;
			}
		}

		string getType(){
			return "Def";
		}

		void setEpsilon(bool flag){
			epsilon = flag;
		}

};

class RegularSymbols : public RegularExpressionTokens{
	private:
		char symbol;
	public:

		//RegularSymbols() = 0;
		RegularSymbols(char sym){
			switch(sym){
				case '|':
				case '+':
				case '*':
					symbol = sym;
				//default:
					//exitWithMessage("Error in Rules");
			}

		}
		char getSymbol(){
			return symbol;
		}

		string getType(){
			return "Sym";
		}
};
/*
class RegularExpression{
	private:
		vector<RegularExpressionTokens> exp;
};
class LexicalParser{
	private:
		FILE* RulesFile = NULL;
		string error = "";
		bool fileEnd;
	public:
		bool setFile(string fileName){
			//try to open the file
			RulesFile  = fopen(fileName.c_str(),"r");
			fileEnd = false;
			//if failed, set the error message and return false
			if(RulesFile == NULL){
				error = "Can't open the Rules file for reading.";
				return false;
			}
		}
		return true;
		string getError(){
			return error;
		}
		string readNextLine(){
			string line;
			char c;
		    // iteratively get characters from standard input, checking for CR (Mac OS), LF (Linux), and CRLF (Windows)
		    while ((c = fgetc(RulesFile)) != '\r' && c != '\n' && c != EOF){
		    	line += c;
		    }
		    if(c == EOF){
		    	fileEnd = true;
		    }
		    return line;
		}
		bool parseFile(){
			while(!fileEnd){
				line = readNextLine();
			}
		}
		//vector<RegularExpression> getExpressions();
};
*/
class Node{
    private:
        bool acceptance;
        int label;

    public:
        Node(int index) {
        	label = index;
        }
        vector<pair<RegularDefinition , Node> > transition;
        void setState(bool acceptanceFlag)  {
        	acceptance = acceptanceFlag;
        }
        bool getState() {
        	return acceptance;
        }
        int getLabel() {
        	return label;
        }

};
class NFA{

    private:
        vector<RegularExpressionTokens*> expression;
        Node startingPoint = NULL;
        Node endingPoint = NULL;
        RegularDefinition epsilon;
        stack< pair <RegularDefinition,Node> >s;
        int index =0;
        RegularDefinition trivial;

	public:
		void setExpressions(vector<RegularExpressionTokens*> rE) {
			expression = rE;
		}
		Node getStartingNode() {
			return startingPoint;
		}
		void convertToNFA(){
			epsilon.setEpsilon(true);
			epsilon.setDefinitionName("Epsilon");
			for(int i=0;i<expression.size();i++){
				if((*expression[i]).getType()=="Def"){

					RegularDefinition* temp= (RegularDefinition*) &(*expression[i]);
					Node first= Node(index++);
					Node second= Node(index++);
					first.transition.push_back(make_pair( *temp,second));
					s.push(make_pair(*temp ,first));
				}
				else {
					RegularSymbols* temp= (RegularSymbols*) &(*expression[i]);
					if(temp->getSymbol()=='|'){
						pair<RegularDefinition,Node> two=s.top();
						s.pop();
						pair<RegularDefinition,Node> one = s.top();
						s.pop();
						oring(&(one.second) , &(one.second.transition[0].second) , &(two.second) ,&(two.second.transition[0].second));
					}
					else if(temp->getSymbol()=='*'){
                        pair<RegularDefinition,Node> one=s.top();
                        s.pop();
                        if(index==2){
                            startingPoint=one.second;
                            endingPoint=one.second.transition[0].second;
                            cout<<endingPoint.getLabel();
                            cout<<"here";
                        }
                        kleen_clouser(&startingPoint,&endingPoint);
					}
					else if(temp->getSymbol()=='+'){
                        pair<RegularDefinition,Node> one=s.top();
                        if(index==2){
                            startingPoint=one.second;
                            endingPoint=startingPoint.transition[0].second;
                        }
                        s.pop();
                        positive_clouser(one.second,endingPoint);
					}
				}
			}
          //  cout<<index;
		}
		void oring (Node* startA , Node* endA , Node* startB , Node* endB){
			//Node* temp = &endA;
			//Node* tempx = &endB;

			Node* temp1 = new Node(index++);
			Node* temp2 =  new Node(index++);
			//temp->transition.push_back(make_pair(epsilon , temp2));
			//tempx->transition.push_back(make_pair(epsilon , temp2));
//
            endA->transition.push_back(make_pair(epsilon , *temp2));
            endB->transition.push_back(make_pair(epsilon , *temp2));
            cout<<"here = "<<startA->transition[0].second.transition.size()<<endl;
            cout<<"here2 = "<<endA->transition.size()<<endl;

            temp1->transition.push_back(make_pair(epsilon , *startA));
            temp1->transition.push_back(make_pair(epsilon , *startB));


            startingPoint = *temp1;
            endingPoint = *temp2;

            s.push(make_pair(trivial,startingPoint));
		}
		void kleen_clouser(Node* startA , Node* endA  ){
            Node* temp1 = new Node(index++);
			Node* temp2 =  new Node(index++);
			endA->transition.push_back(make_pair(epsilon,*startA));
			endA->transition.push_back(make_pair(epsilon,*temp2));
			temp1->transition.push_back(make_pair(epsilon,endA->transition[endA->transition.size()-1].second));
			temp1->transition.push_back(make_pair(epsilon,*temp2));
			startingPoint=*temp1;
			endingPoint=*temp2;
			s.push(make_pair(trivial,startingPoint));
        }
        void positive_clouser(Node startA , Node endA  ){
            Node temp1 =  Node(index++);
			Node temp2 =  Node(index++);
			endA.transition.push_back(make_pair(epsilon,startA));
			temp1.transition.push_back(make_pair(epsilon,startA));
			endA.transition.push_back(make_pair(epsilon,temp2));
			startingPoint=temp1;
			endingPoint=temp2;
			s.push(make_pair(trivial,startingPoint));
        }
};


/*
class DFA{
	public:
		bool setNFA(NFAState);
		bool convertToDFA();
		DFATable getDFATable();
};
class LexicalScanner{
	public:
		bool setDFA(DFATable);
		bool setCode(string);
		Token getNextTocken();
};
void exitWithMessage(string msg){
	cout << msg << endl;
	exit(9);
}
*/
bool vis[100];
void DFS(Node Start ,bool vis[]){
    cout<<Start.getLabel()<<"  "<<Start.transition.size()<<endl;
  //cout<<Start.getLabel()<<endl;
    vis[Start.getLabel()]=true;
    for(int i=0;i<Start.transition.size();i++){
        if(!vis[Start.transition[i].second.getLabel()]){

                //cout<<Start.getLabel()<<"  "<< Start.transition[i].first.getDefinitionName() <<"  "<<Start.transition[i].second.getLabel()<<endl;

                DFS(Start.transition[i].second,vis);
        }
    }
}
int main(){

	/*if(argc != 3){
		exitWithMessage("you can only call the program with\n./Lexical RulesFile CodeFile.");
	}
	LexicalParser parser;
	//open the Rules file
	if(!parser.setFile(argv[1])){
		//if there is error, print the error and exit
		exitWithMessage(parser.getError());
	}
	cout << "Hello, World" << endl;*/

        fill(vis,vis+100,false);
		RegularDefinition letter;
        letter.setDefinitionName("letter");

		RegularDefinition digit ;
		digit.setDefinitionName("digit");

        RegularDefinition epsilon ;
        epsilon.setDefinitionName("Epsilon");

		RegularSymbols Or = RegularSymbols('*');
		vector<RegularExpressionTokens*> v ={(RegularExpressionTokens*)(&digit),(RegularExpressionTokens*)(&Or)};
		NFA nfa;
		nfa.setExpressions(v);
		nfa.convertToNFA ();
        DFS(nfa.getStartingNode(),vis);
        return 0;
}
