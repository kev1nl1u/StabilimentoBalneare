// "Stabilimento Balneare" v2.0 Winter Update
/***********************
 * author: Kevin Liu
 * @kev1nl1u
 * github.com/kev1nl1u
 ***********************/

/***** WARNINGS *****/
#ifdef __APPLE__
	#warning "This program may not run correctly on Apple systems!"
#endif
#ifdef __linux__
	#warning "This program may not run correctly on Linux systems!"
#endif

/***** LIBRARIES *****/
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>		// std::ostringstream
#include <iomanip>      // std::setprecision
#include "lib/Group"
#include "lib/Rink"

/***** CONSTANTS *****/
#define MAX_GROUPS_GENERATED 15
#define MAX_PEOPLE_IN_GROUP 10
#define SKATE_SIZE_MIN 24
#define SKATE_SIZE_MAX 44
#define ONE_SECOND_IN_SIMULATION_MINUTE 1
#define UPDATE_TIMEOUT_MS 5000

using namespace std;

/***** VARIABLES *****/
// variable used for style and log purposes
bool style_printEndline, log, logOnFile, logPrinted, logPrintedOnFile;
string logString, statsString;
ofstream logFile;

/***** FUNCTIONs *****/
// bool true/false to string ON/OFF
string boolToOnOff(bool b){
	if(b) return "ON";
	else return "OFF";
}
// double to string with 2 decimal places
string roundDoubleToString(double n) {
    ostringstream s;
    s << std::fixed << std::setprecision(2) << n;
    return s.str();
}
// DETECT KEY PRESSES
// detect ESC key (exit program)
void detectEsc(){
	if (GetAsyncKeyState(VK_ESCAPE)){
      		if(!style_printEndline){ cout << endl; }
		cout << "Exiting..." << endl; exit(0);
	}
}
// detect F1 key (print log in console)
void detectF1(){
	if (GetAsyncKeyState(VK_F1)){
		log = !log;
		if(!style_printEndline && !log){ cout << endl; style_printEndline = true; }
		if(log) cout << "Log in console enabled" << endl; else cout << "Log in console disabled" << endl;
		if(log && !logPrinted){ cout << logString; logPrinted = true; }
	}
}
// detect F2 key (print log on file)
void detectF2(){
	if (GetAsyncKeyState(VK_F2)){
		logOnFile = !logOnFile;
		if(!style_printEndline){ cout << endl; style_printEndline = true; }
		if(logOnFile) cout << "Log on file enabled" << endl; else cout << "Log on file disabled" << endl;
		if(log && !logPrintedOnFile){
			logFile << endl << "-----" << endl << statsString << logString;
			logPrintedOnFile = true;
		}
	}
}
// detect F3 key (pause/resume program)
void detectF3(){
	if (GetAsyncKeyState(VK_F3)){
		if(!style_printEndline){ cout << endl; style_printEndline = true; }
		cout << "Pause" << endl;
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
		cout << "Resume" << endl;
	}
}

/***** MAIN *****/
int main(){

	srand(time(NULL));
	
	// queue of groups waiting to enter the track
	vector<Group> queue;

	// create ice rink
	Rink iceRink = Rink();

	// variables
	int time = 0; // time
	log = true; // print log in console
	logOnFile = true; // print log on file
	int id = 0; // id of the group generated
	logFile.open("log.txt", ios::out); // open log file

	// while user doesn't press ESC
	while(true){
		
		Sleep(50); // too much stuff to print, cpu needs to think a bit

		// reset string for log and stats
		logString = "";
		statsString = "";

		logString += "LOG: \n";

		// check if there are groups that have to exit
		for(int i=0; i<iceRink.getNGroups(); i++){
			// random 1 = group exits
			if(rand()%2){
				iceRink.groups[i].setTimeExit(time); // set time of exit

				// update log
				logString += "- OUT group ID" + to_string(iceRink.getGroup(i).getID()) + " from rink (-" + to_string(iceRink.getPeopleInGroup(i)) + " people) ";
				// add to log the skate returned from each person in the group
				logString += "(SIZE/ID: ";
				for(int j=0; j<iceRink.getGroup(i).getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(iceRink.getGroup(i).getSize(j)) + " ID" + to_string(iceRink.getGroup(i).getID());
				}
				// time spent in the rink
				logString += ") - TimeIn: " + to_string(iceRink.getGroup(i).getTimeEnter()) + "-" + to_string(iceRink.getGroup(i).getTimeExit()) + " ";
				logString += "+" + roundDoubleToString(iceRink.calcRevenue(iceRink.getGroup(i))) + " euro\n";
				// remove group from rink
				iceRink.removeGroup(i);
				// decrement i because the vector has been modified
				i--;
			}
		}

		// generate new groups
		for(int i=0; i<rand()%MAX_GROUPS_GENERATED+1; i++){
			// random number of people in the group
			int nPeopleInGroup = rand()%MAX_PEOPLE_IN_GROUP+1;
			// create group
			Group tempGroup = Group(nPeopleInGroup, id);
			id++; // increment id
			// update log
			logString += "- NEW group ID" + to_string(tempGroup.getID()) + " of " + to_string(nPeopleInGroup) + " people\n";
			// push in queue
			queue.push_back(tempGroup);
			// update log
			logString += "- ADD group ID" + to_string(tempGroup.getID()) +" to queue\n";
		}

		// check if a group can enter the rink
		for(int i=0; i<queue.size(); i++){
			// if there is enough space in the rink and there are enough skates
			if(iceRink.canAddGroup(queue[i]) && iceRink.skates.checkAvailabilityForGroup(queue[i])){
				// remove skates from stock
				iceRink.skates.removeStockForGroup(queue[i]);
				// update log
				logString += "- AVAIL skates for group ID" + to_string(queue[i].getID()) + " in queue (";
				for(int j=0; j<queue[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(queue[i].getSize(j));
				}
				logString += ")\n";
				// set time of entry
				queue[i].setTimeEnter(time);
				// add group to rink
				iceRink.addGroup(queue[i]);
				// update log
				logString += "- IN group ID" + to_string(queue[i].getID()) + " (+" + to_string(queue[i].getNPeople()) + " people) (queue: " + to_string(queue.size()) + " group(s)) ";
				// add to log the skate taken by each person in the group
				logString += "(SIZE/ID: ";
				for(int j=0; j<queue[i].getNPeople(); j++){
					if(j!=0) logString += "; ";
					logString += to_string(queue[i].getSize(j)) + " ID" + to_string(queue[i].getSkateID(j));
				}
				logString += ")\n";
				// remove group from queue
				queue.erase(queue.begin()+i);
				// decrement i because the vector has been modified
				i--;
			}
		}

		// update log with groups in rink
		logString += "- Groups in rink: \n";
		for(int i=0; i<iceRink.getNGroups(); i++){
			logString += "\tID" + to_string(iceRink.getGroup(i).getID()) + " - " + to_string(iceRink.getPeopleInGroup(i)) + " people; ";
			logString += "(SIZE/ID: ";
			for(int j=0; j<iceRink.getGroup(i).getNPeople(); j++){
				if(j!=0) logString += "; ";
				logString += to_string(iceRink.getGroup(i).getSize(j)) + "ID" + to_string(iceRink.getGroup(i).getSkateID(j));
			}
			logString += ")\n";
		}
		// add to log rentalsLeft of all skates
		logString += "\n\tSkates rental left: \n";
		// for each size
		for(int i = iceRink.skates.getSizeStart(); i<=iceRink.skates.getSizeEnd(); i++){
			logString += "\tSize " + to_string(i) + ":\t";
			// for each skate of that size
			for(int j=0; j<iceRink.skates.skateSize->getNSkates(); j++){
				// if the skate is in maintenance
				if(iceRink.skates.skateSize[i-iceRink.skates.getSizeStart()].isInMaintenance(j)){
					// show M
					logString += " M";
				}else{
					// else show rentals left before maintenance
					char buffer[3];
					sprintf(buffer, "%2d", iceRink.skates.skateSize[i-iceRink.skates.getSizeStart()].getRentalsLeftOf(j));
					logString += buffer;
				}
				logString += "    ";
			}
			logString += "\n";
		}

		// update stats
		statsString += "Time: " + to_string(time) + " minutes\n\n";
		statsString += "Rink: " + to_string(iceRink.getPeople()) + " / " + to_string(iceRink.getMax()) + "\n";
		statsString += "in " + to_string(iceRink.getNGroups()) + " groups\n\n";
		statsString += "Skates status: \n";
		int boxCount = 1;
		for(int i = iceRink.skates.getSizeStart(); i<=iceRink.skates.getSizeEnd(); i++){
			if(boxCount%2) statsString += "\t" + to_string(i) + "\t" + to_string(iceRink.skates.getScorte(i)) + " / 6\t";
			else statsString += "\t" + to_string(i) + "\t" + to_string(iceRink.skates.getScorte(i)) + " / 6\t";
			for(int j=0; j<iceRink.skates.skateSize->getNSkates(); j++){
				if(iceRink.skates.skateSize[i-iceRink.skates.getSizeStart()].isInMaintenance(j)) statsString += "M";
				else if(iceRink.skates.skateSize[i-iceRink.skates.getSizeStart()].isAvailable(j)) statsString += "X";
				else statsString += "O";
			}
			if(boxCount%2) statsString += "\t\t";
			else statsString += "\n";
			boxCount++;
		}
		if((boxCount-1)%2) statsString += "\n";
		statsString += "\n";
		int nPeopleInQueue = 0;
		for(int i=0; i<queue.size(); i++) nPeopleInQueue += queue[i].getNPeople();
		statsString += "Queue: " + to_string(queue.size()) + " groups (" + to_string(nPeopleInQueue) + " people)\n";
		for(int i=0; i<queue.size(); i++){
			statsString += "\tID" + to_string(queue[i].getID()) + ": ";
			for(int j=0; j<queue[i].getNPeople(); j++){
				if(j!=0) statsString += "; ";
				statsString += to_string(queue[i].getSize(j));
			}
			statsString += "\n";
		}
		statsString += "\n";
		statsString += "REVENUE: " + roundDoubleToString(iceRink.getRevenue()) + " euro\n";
		statsString += "EXPENSES: " + roundDoubleToString(iceRink.getExpense()) + " euro\n\n";

		// print stats
		cout << statsString;

		// print commands
		cout << "ESC to quit" << endl << "F1 to print log in console\t\t" << boolToOnOff(log) << endl << "F2 to print log on file\t\t\t" << boolToOnOff(logOnFile) << endl << "F3 to pause/resume" << endl << endl;
		
		// print log
		logPrinted = false;
		if(log){ cout << logString; logPrinted = true; }
		logPrintedOnFile = false;
		if(logOnFile){ logFile << endl << "-----" << endl << statsString << logString; logPrintedOnFile = true; }
		
		style_printEndline = false;

		// wait 5 seconds (1 second = 5 minutes in the simulation)
		// check for input
		for(int i=0; i<20; i++){
        	detectEsc();
			detectF1();
			detectF2();
			detectF3();
			Sleep(UPDATE_TIMEOUT_MS/20);
		}

		// update time
		time+=5*ONE_SECOND_IN_SIMULATION_MINUTE;
		iceRink.updateTime(time);

		// clear screen
		system("cls");
    }
    
	logFile.close();
	
	
	
	

	return 0;
}
