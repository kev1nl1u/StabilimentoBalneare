// Stabilimento Balneare v2.0 Inverno Update

/*
 * notes:
 * return value 3221226356 after running for a while (allocation error?)
 * return value 3221225477 after running for a while (out of range error?)
 */

#ifdef __APPLE__
	#warning "This program may not run correctly on Apple systems!"
#endif
#ifdef __linux__
	#warning "This program may not run correctly on Linux systems!"
#endif

#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>      // std::setprecision
#include "lib/Group"
#include "lib/Track"

using namespace std;

string boolToOnOff(bool b){
	if(b) return "ON";
	else return "OFF";
}

string roundDoubleToString(double n) {
    ostringstream s;
    s << std::fixed << std::setprecision(2) << n;
    return s.str();
}

int main(){

	srand(time(NULL));
	
	vector<Group> coda; // coda delle persone

	// inizializza pista
	Track pista = Track();

	int tempo = 0;
	int nPeopleInGroup;
	bool log = true;
	bool logOnFile = true;
	int id = 0;
	
	ofstream logFile;
	logFile.open("log.txt", ios::out);

	// while user input != esc
	while(true){
		
		Sleep(100); // troppa roba da stampare, 50s di sleep per far ragionare il pc

		string logString = "";
		string statsString = "";

		logString += "LOG: \n";

		// controlla se ci sono gruppi in uscita
		for(int i=0; i<pista.getNGroups(); i++){
			if(rand()%2){ // random 1 = gruppo esce

				pista.setTimeExitOfGroup(i, tempo); // pista.getGroup(i).setTimeExit(tempo);

				logString += "- OUT gruppo {" + to_string(pista.getGroup(i).getID()) + "} dalla pista (" + to_string(pista.getPeopleInGroup(i)) + " persone) (taglie ";
				for(int j=0; j<pista.getPeopleInGroup(i); j++){
					if(j!=0) logString += "; ";
					logString += to_string(pista.getGroup(i).getSize(j));
				}
				logString += ") - TimeIn: " + to_string(pista.getGroup(i).getTimeEnter()) + "-" + to_string(pista.getGroup(i).getTimeExit()) + " ";
				logString += "+" + roundDoubleToString(pista.calcRevenue(pista.getGroup(i))) + " euro\n";

				pista.removeGroup(i);

			}
		}

		// genera max 10 gruppi
		for(int i=0; i<rand()%10+1; i++){
			nPeopleInGroup = rand()%10+1; // genera numero casuale di persone (1-10)
			Group tempGroup = Group(nPeopleInGroup, id);
			id++;
			logString += "- NEW gruppo {" + to_string(tempGroup.getID()) + "} di " + to_string(nPeopleInGroup) + " persone\n";
			coda.push_back(tempGroup); // push in coda
			logString += "- ADD gruppo {" + to_string(tempGroup.getID()) +"} alla coda\n";
		}

		// controlla che ci sia spazio nella pista per i gruppi in coda
		for(int i=0; i<coda.size(); i++){
			if(pista.canAddGroup(coda[i])){

				logString += "- AVAIL taglie gruppo {" + to_string(coda[i].getID()) + "} in coda disponibili (";
				for(int j=0; j<coda[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(coda[i].getSize(j));
				}
				logString += ")\n";

				pista.addGroup(coda[i], tempo);

				logString += "- IN gruppo {" + to_string(coda[i].getID()) + "} (" + to_string(coda[i].getNPeople()) + " persone) (coda: " + to_string(coda.size()) + " gruppo)\n";
				
				coda.erase(coda.begin()+i);
				i--;
			}/*else if(!pattini.checkAvailabilityForGroup(coda[i])){
				logString += "- Taglie gruppo {" + to_string(coda[i].getID()) + "} della coda non disponibili (";
				for(int j=0; j<coda[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(coda[i].getSize(j));
				}
				logString += ")\n";
			}else if(!pista.canAddGroup(coda[i])){
				logString += "- Gruppo {" + to_string(coda[i].getID()) + "} della coda non entrato (pista piena)\n";
			}*/
		}

		// gruppi in pista in log
		logString += "- PISTA gruppi: \n";
		for(int i=0; i<pista.getNGroups(); i++){
			logString += "\t{" + to_string(pista.getGroup(i).getID()) + "} - " + to_string(pista.getPeopleInGroup(i)) + " persone - Enter: " + to_string(pista.getGroup(i).getTimeEnter()) + ";\n";
		}

		// calcola e stampa stats
		statsString += "Tempo: " + to_string(tempo) + " minuti\n\n";
		statsString += "Pista: " + to_string(pista.getPeople()) + " / " + to_string(pista.getMax()) + "\n";
		statsString += "composto da " + to_string(pista.getNGroups()) + " gruppi\n\n";
		statsString += "Pattini: [X: available; O: in use; M: maintenance]\n";
		int boxCount = 1;
		for(int i = pista.pattini.getSizeStart(); i<=pista.pattini.getSizeEnd(); i++){
			statsString += "\t" + to_string(i) + "\t" + to_string(pista.pattini.getStockOfSize(i)) + " / 6\t";
			for(int j=0; j<6; j++){
				if(pista.pattini.skate[i-pista.pattini.getSizeStart()][j].isInMaintenance()) statsString += "M";
				else if(pista.pattini.skate[i-pista.pattini.getSizeStart()][j].isAvailable()) statsString += "X";
				else statsString += "O";
			}
			if(boxCount%2) statsString += "\t\t";
			else statsString += "\n";
			boxCount++;
		}
		if((boxCount-1)%2) statsString += "\n";
		statsString += "\n";

		int personeInCoda = 0;
		for(int i=0; i<coda.size(); i++){
			personeInCoda += coda[i].getNPeople();
		}
		statsString += "Coda: " + to_string(coda.size()) + " gruppi (" + to_string(personeInCoda) + ")\n\n";
		statsString += "FATTURATO: " + roundDoubleToString(pista.getRevenue()) + " euro\n";
		statsString += "SPESE: " + roundDoubleToString(pista.getExpense()) + " euro\n\n";

		cout << statsString;

		cout << "ESC per uscire" << endl << "F1 per attivare/disattivare log\t\t" << boolToOnOff(log) << endl << "F2 per stampare log su file\t\t" << boolToOnOff(logOnFile) << endl << "F3 per pausa/riprendi" << endl << endl;

		bool logPrinted = false;
		if(log){ cout << logString; logPrinted = true; }
		bool logPrintedOnFile = false;
		if(logOnFile){ logFile << endl << "-----" << endl << statsString << logString; logPrintedOnFile = true; }
		
		bool style_printEndline = false;
		for(int i=0; i<20; i++){
        	if (GetAsyncKeyState(VK_ESCAPE)){
        		if(!style_printEndline){ cout << endl; }
				cout << "Exiting..." << endl; return 0;
			}
			if (GetAsyncKeyState(VK_F1)){
				log = !log;
				if(!style_printEndline && !log){ cout << endl; style_printEndline = true; }
				if(log) cout << "Log attivato" << endl; else cout << "Log disattivato" << endl;
				if(log && !logPrinted){ cout << logString; logPrinted = true; }
			}
			if (GetAsyncKeyState(VK_F2)){
				logOnFile = !logOnFile;
				if(!style_printEndline){ cout << endl; style_printEndline = true; }
				if(log) cout << "Stampa log su file" << endl; else cout << "Non stampare log su file" << endl;
				if(log && !logPrintedOnFile){
					logFile << endl << "-----" << endl << statsString << logString;
					logPrintedOnFile = true;
				}
			}
			if (GetAsyncKeyState(VK_F3)){
				if(!style_printEndline){ cout << endl; style_printEndline = true; }
				cout << "Pausa" << endl;
				fflush(stdin);
				Sleep(50);
				while(true){
					if (GetAsyncKeyState(VK_ESCAPE)){
		        		if(!style_printEndline){ cout << endl; }
						cout << "Exiting..." << endl;
						logFile.close();
						return 0;
					}
					if (GetAsyncKeyState(VK_F1)){
						log = !log;
						if(!style_printEndline && !log){ cout << endl; style_printEndline = true; }
						if(log) cout << "LOG attivato" << endl; else cout << "LOG disattivato" << endl;
						if(log && !logPrinted){ cout << endl << logString; logPrinted = true; }
					}
					if (GetAsyncKeyState(VK_F2)){
						logOnFile = !logOnFile;
						if(!style_printEndline){ cout << endl; style_printEndline = true; }
						if(log) cout << "Stampa log su file" << endl; else cout << "Non stampare log su file" << endl;
						if(log && !logPrintedOnFile){
							logFile << endl << "-----" << endl << statsString << logString;
							logPrintedOnFile = true;
						}
					}
					if (GetAsyncKeyState(VK_F3)) break;
				}
				cout << "Riprendi" << endl;
			}
			Sleep(250);
		}
		tempo+=5;
		pista.updateTime(tempo);
		system("cls");
    }
    
	logFile.close();
	
	
	
	

	return 0;
}
