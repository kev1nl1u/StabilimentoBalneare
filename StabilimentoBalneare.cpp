// Stabilimento Balneare v2.0 Inverno Update

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

#define MAX_GRUPPI_GENERATI 20
#define MAX_PERSONE_IN_GRUPPO 10

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

// style var
bool style_printEndline, log, logOnFile, logPrinted, logPrintedOnFile;
string logString, statsString;
ofstream logFile;

void detectEsc(){
	if (GetAsyncKeyState(VK_ESCAPE)){
      		if(!style_printEndline){ cout << endl; }
		cout << "Exiting..." << endl; exit(0);
	}
}
void detectF1(){
	if (GetAsyncKeyState(VK_F1)){
		log = !log;
		if(!style_printEndline && !log){ cout << endl; style_printEndline = true; }
		if(log) cout << "Log in console attivato" << endl; else cout << "Log in console disattivato" << endl;
		if(log && !logPrinted){ cout << logString; logPrinted = true; }
	}
}
void detectF2(){
	if (GetAsyncKeyState(VK_F2)){
		logOnFile = !logOnFile;
		if(!style_printEndline){ cout << endl; style_printEndline = true; }
		if(logOnFile) cout << "Stampa log su file" << endl; else cout << "Non stampare log su file" << endl;
		if(log && !logPrintedOnFile){
			logFile << endl << "-----" << endl << statsString << logString;
			logPrintedOnFile = true;
		}
	}
}
void detectF3(){
	if (GetAsyncKeyState(VK_F3)){
		if(!style_printEndline){ cout << endl; style_printEndline = true; }
		cout << "Pausa" << endl;
		fflush(stdin);
		Sleep(50);
		while(true){
			detectEsc();
			Sleep(50);
			detectF1();
			Sleep(50);
			detectF2();
			Sleep(50);
			if (GetAsyncKeyState(VK_F3)) break;
		}
		cout << "Riprendi" << endl;
	}
}

int main(){

	srand(time(NULL));
	
	vector<Group> coda; // coda delle persone

	// inizializza pista
	Track pista = Track();

	int tempo = 0;
	log = true;
	logOnFile = true;
	int id = 0;
	
	logFile.open("log.txt", ios::out);

	// while user input != esc
	while(true){
		
		Sleep(50); // troppa roba da stampare, 50s di sleep per far ragionare il pc

		logString = "";
		statsString = "";

		logString += "LOG: \n";

		// controlla se ci sono gruppi in uscita
		for(int i=0; i<pista.getNGroups(); i++){
			if(rand()%2){ // random 1 = gruppo esce
				pista.gruppi[i].setTimeExit(tempo);
				logString += "- OUT gruppo ID" + to_string(pista.getGroup(i).getID()) + " dalla pista (" + to_string(pista.getPeopleInGroup(i)) + " persone) ";
				// print taglie e id
				logString += "(TAGLIA ID: ";
				for(int j=0; j<pista.getGroup(i).getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(pista.getGroup(i).getSize(j)) + " ID" + to_string(pista.getGroup(i).getID());
				}
				logString += ") - TimeIn: " + to_string(pista.getGroup(i).getTimeEnter()) + "-" + to_string(pista.getGroup(i).getTimeExit()) + " ";
				logString += "+" + roundDoubleToString(pista.calcRevenue(pista.getGroup(i))) + " euro\n";
				pista.removeGroup(i);
				i--;
			}
		}

		// genera gruppi
		for(int i=0; i<rand()%MAX_GRUPPI_GENERATI+1; i++){
			int nPeopleInGroup = rand()%MAX_PERSONE_IN_GRUPPO+1; // genera numero casuale di persone
			Group tempGroup = Group(nPeopleInGroup, id);
			id++;
			logString += "- NEW gruppo ID" + to_string(tempGroup.getID()) + " di " + to_string(nPeopleInGroup) + " persone\n";
			coda.push_back(tempGroup); // push in coda
			logString += "- ADD gruppo ID" + to_string(tempGroup.getID()) +" alla coda\n";
		}

		// controlla che ci sia spazio nella pista per i gruppi in coda
		for(int i=0; i<coda.size(); i++){
			if(pista.canAddGroup(coda[i]) && pista.pattini.checkAvailabilityForGroup(coda[i])){
				pista.pattini.removeStockForGroup(coda[i]);
				logString += "- AVAIL taglie gruppo ID" + to_string(coda[i].getID()) + " in coda disponibili (";
				for(int j=0; j<coda[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(coda[i].getSize(j));
				}
				logString += ")\n";
				coda[i].setTimeEnter(tempo);
				pista.addGroup(coda[i]);
				logString += "- IN gruppo ID" + to_string(coda[i].getID()) + " (" + to_string(coda[i].getNPeople()) + " persone) (coda: " + to_string(coda.size()) + " gruppo) ";
				// print taglie e id
				logString += "(TAGLIA ID: ";
				for(int j=0; j<coda[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(coda[i].getSize(j)) + " ID" + to_string(coda[i].getSkateID(j));
				}
				logString += ")\n";
				coda.erase(coda.begin()+i);
				i--;
			}/*else if(!pista.pattini.checkAvailabilityForGroup(coda[i])){
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

		// stampa gruppi in pista
		logString += "- PISTA gruppi: \n";
		for(int i=0; i<pista.getNGroups(); i++){
			logString += "\tID" + to_string(pista.getGroup(i).getID()) + " - " + to_string(pista.getPeopleInGroup(i)) + " persone; ";
			// print taglie e id
			logString += "(TAGLIA ID: ";
			for(int j=0; j<pista.getGroup(i).getNPeople(); j++){
				if(j!=0) logString += "; ";
				logString += to_string(pista.getGroup(i).getSize(j)) + "ID" + to_string(pista.getGroup(i).getSkateID(j));
			}
			logString += ")\n";
		}

		// calcola e stampa stats
		statsString += "Tempo: " + to_string(tempo) + " minuti\n\n";
		statsString += "Pista: " + to_string(pista.getPeople()) + " / " + to_string(pista.getMax()) + "\n";
		statsString += "composto da " + to_string(pista.getNGroups()) + " gruppi\n\n";
		statsString += "Pattini: \n";
		int boxCount = 1;
		for(int i = pista.pattini.getSizeStart(); i<=pista.pattini.getSizeEnd(); i++){
			if(boxCount%2) statsString += "\t" + to_string(i) + "\t" + to_string(pista.pattini.getScorte(i)) + " / 6\t\t";
			else statsString += "\t" + to_string(i) + "\t" + to_string(pista.pattini.getScorte(i)) + " / 6\t";
			for(int j=0; j<pista.pattini.scorte->getNSkates(); j++){
				if(pista.pattini.scorte[i-pista.pattini.getSizeStart()].isInMaintenance(j)) statsString += "M";
				else if(pista.pattini.scorte[i-pista.pattini.getSizeStart()].isAvailable(j)) statsString += "X";
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

		cout << "ESC per uscire" << endl << "F1 per stampare log su console\t\t" << boolToOnOff(log) << endl << "F2 per stampare log su file\t\t" << boolToOnOff(logOnFile) << endl << "F3 per pausa/riprendi" << endl << endl;

		logPrinted = false;
		if(log){ cout << logString; logPrinted = true; }
		logPrintedOnFile = false;
		if(logOnFile){ logFile << endl << "-----" << endl << statsString << logString; logPrintedOnFile = true; }
		
		style_printEndline = false;
		for(int i=0; i<20; i++){
        	detectEsc();
			detectF1();
			detectF2();
			detectF3();
			Sleep(250);
		}

		tempo+=5;
		pista.updateTime(tempo);
		system("cls");
    }
    
	logFile.close();
	
	
	
	

	return 0;
}
